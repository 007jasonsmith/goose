#include "shell/shell.h"

#include "hal/keyboard.h"
#include "hal/text_ui.h"
#include "kernel/elf.h"
#include "kernel/boot.h"
#include "klib/debug.h"
#include "klib/limits.h"
#include "klib/types.h"
#include "klib/panic.h"
#include "klib/strings.h"

using hal::Color;
using hal::TextUI;
using hal::Keyboard::KeyPress;

namespace {

struct ShellCommand {
  const char* command;
  void (*func)(shell::ShellStream* shell);
};

// Initialize shell chrome.
void InitializeChrome();
// Show a map of the machine's memory.
void ShowMemoryMap(shell::ShellStream* shell);
// Print some pointers for kernel code. (Figure out where it exists.)
void ShowKernelPointers(shell::ShellStream* shell);
// Print all known information about the Kernel's ELF binary.
void ShowElfInfo(shell::ShellStream* shell);
// Experimental code.
void Experiment(shell::ShellStream* shell);
// Returns the command with the name, otherwise null.
const ShellCommand* GetShellCommand(const char* command);

const ShellCommand commands[] = {
  { "show-memory-map", &ShowMemoryMap },
  { "show-kernel-pointers", &ShowKernelPointers },
  { "show-elf-info", &ShowElfInfo },
  { "experiment", &Experiment }
};
const size kNumCommands = sizeof(commands) / sizeof(ShellCommand);

void InitializeChrome() {
  // Title
  for (int i = 0; i < 80; i++) {
    TextUI::SetColor(i, 0, Color::White, Color::Red);
  }
  const char* title = " Goose v1.0a";
  for (int i = 0; i < klib::length(title); i++) {
    TextUI::SetChar(i, 0, title[i]);
  }

  // Rest of the screen.
  for (int y = 1; y < 25; y++) {
    for (int x = 0; x < 80; x++) {
      TextUI::SetChar(x, y, ' ');
      TextUI::SetColor(x, y, Color::Gray, Color::Black);
    }
  }
}

void ShowMemoryMap(shell::ShellStream* shell) {
  const char* kRegionNames[] = {
    "Unknown",
    "Usable",
    "Reserved",
    "ACPI reclaimable",
    "ACPI NVS memory",
    "Bad memory",
    // Not an official value. Used here to mark regions not
    // specified in the memory map.
    "Reserved (Unspecified)"
  };

  // Keep track of regions which are usable / reclaimable.
  size usable_regions_count = 0;
  kernel::grub::multiboot_memory_map* usable_regions[10];

  const kernel::grub::multiboot_info* mbt = kernel::GetMultibootInfo();

  kernel::grub::multiboot_memory_map* mmap =
    (kernel::grub::multiboot_memory_map*) mbt->mmap_addr;

  // TODO(chris): Sanity check 32-bits for the time being.
  shell->WriteLine("System memory:");
  // Lower means < 1MiB?
  shell->WriteLine("  lower %dKiB / %dMiB", mbt->mem_lower, mbt->mem_lower / 1024);
  shell->WriteLine("  upper %dKiB / %dMiB", mbt->mem_upper, mbt->mem_upper / 1024);
  shell->WriteLine("  total %dKiB / %dMiB",
		   mbt->mem_lower + mbt->mem_upper,
		   (mbt->mem_lower + mbt->mem_upper) / 1024);

  // Print memory regions. We do additional book keeping to print unspecified
  // regions (assumed reserved). We assume regions are sorted, but that isn't
  // guaranteed by the BIOS.
  uint32 last_region_end = 0x00000000;
  shell->WriteLine("Memory regions:");
  while((uint32) mmap < (uint32) mbt->mmap_addr + mbt->mmap_length) {
    // Sanity check the memory map region.
    // - Check 32-bit ranges
    // - Check it doesn't contain ACPI 3.0 Extended attributes
    // - Check type value is correct
    if (mmap->base_addr_high) { klib::Panic("Non-zero base_addr_high"); }
    if (mmap->length_high) { klib::Panic("Non-zero length_high"); }
    if (mmap->size != 20) { klib::Panic("Memory Map region != 20 bytes"); }
    if (mmap->type == 0 || mmap->type > 5) {
      klib::Panic("Memory map region type is unknown.");
    }

    const uint32 region_start = mmap->base_addr_low;
    const uint32 region_end = mmap->base_addr_low + mmap->length_low;

    if (region_start < last_region_end) {
      klib::Panic("Memory map regions not sorted.");
    }
    // Insert a reserved region as applicable.
    if (region_start != last_region_end) {
      shell->WriteLine("  %h - %h %s",
		       last_region_end, region_start - 1, kRegionNames[6]);
    }
    shell->WriteLine("  %h - %h %s",
		     mmap->base_addr_low, region_end - 1, kRegionNames[mmap->type]);
    last_region_end = region_end;

    // Save the usable memory for elsewhere.
    if (mmap->type == 1 || mmap->type == 3) {
      usable_regions[usable_regions_count] = mmap;
      usable_regions_count++;
    }

    mmap = (kernel::grub::multiboot_memory_map*) (
        (uint32) mmap + mmap->size + sizeof(uint32));
  }
  // Insert a reserved region as applicable. The region ends at the value +1, so
  // the last value should be 0xFFFFFFFF + 1, which is 0.
  if (last_region_end != 0) {
    shell->WriteLine("  %h - 0xFFFFFFFF %s", last_region_end, kRegionNames[6]);
  }

  // Print usable memory.
  shell->WriteLine("Usable regions:");
  for (size i = 0; i < usable_regions_count; i++) {
    mmap = usable_regions[i];

    shell->WriteLine("  %h - %h %dKiB %dMiB",
		     mmap->base_addr_low, mmap->base_addr_low + mmap->length_low -1,
		     mmap->length_low / 1024, mmap->length_low / 1024 / 1024);
  }
}

const double* testing;  // Only referenced by ShowKernelPointers.
void ShowKernelPointers(shell::ShellStream* shell) {
  shell->WriteLine("Function pointer : %h", uint32(&klib::Panic));
  shell->WriteLine("Const data       : %h", uint32(&commands));
  shell->WriteLine("Const data (.bss): %h", uint32(&testing));
}

void ShowElfInfo(shell::ShellStream* shell) {
  const kernel::grub::multiboot_info* mbt = kernel::GetMultibootInfo();
  if ((mbt->flags & 0b100000) == 0) {
    klib::Panic("Multiboot flags bit 5 not set.");
  }

  const kernel::elf::ElfSectionHeaderTable* elf_sec =
    &(mbt->u.elf_sec);
  if (sizeof(kernel::elf::Elf32SectionHeader) != elf_sec->size) {
    klib::Panic("ELF section header doesn't match expected size.");
  }

  shell->WriteLine("ELF sections:");
  shell->WriteLine("  There are %d section headers, starting at %h:",
		   elf_sec->num, elf_sec->addr);

  const kernel::elf::Elf32SectionHeader* string_table_header =
    kernel::elf::GetSectionHeader(elf_sec->addr, elf_sec->shndx);
  if (string_table_header->type != uint32(kernel::elf::SectionType::STRTAB)) {
    klib::Panic("Section header string table is not actually a string table.");
  }

  shell->WriteLine("\nSection Headers:");
  shell->WriteLine("  [Num] %{L16}s %{L12}s %{L10}s %{L10}s %{L10}s Flg",
		   "Name", "Type", "Addr", "Offset", "Size");
  for (size i = 0; i < size(elf_sec->num); i++) {
    const kernel::elf::Elf32SectionHeader* header =
        kernel::elf::GetSectionHeader(elf_sec->addr, i);
    const char* section_name = kernel::elf::GetStringTableEntry(
        string_table_header->addr, string_table_header->size, header->name);
    const char* section_type = kernel::elf::ToString(
	(kernel::elf::SectionType) header->type);

    shell->WriteLine("  [%{R3}d] %{L16:t}s %{L12}s %h %h %h %c%c%c",
		     i, section_name, section_type,
		     header->addr, header->offset, header->size,
		     (header->flags & 0x1 ? 'W' : ' '),
		     (header->flags & 0x2 ? 'A' : ' '),
		     (header->flags & 0x4 ? 'X' : ' '));
  }
}

void Experiment(shell::ShellStream* shell) {
  const kernel::grub::multiboot_info* mbt = kernel::GetMultibootInfo();
  if ((mbt->flags & 0b100000) == 0) {
    klib::Panic("Multiboot flags bit 5 not set.");
  }
  const kernel::elf::ElfSectionHeaderTable* elf_sec = &(mbt->u.elf_sec);

  shell->WriteLine("ELF header info:");
  shell->WriteLine("  Sections = %d, size = %d", elf_sec->num, elf_sec->size);
  shell->WriteLine("  addr = %h / shndx = %h", elf_sec->addr, elf_sec->shndx);

  for (size i = 0; i < size(elf_sec->num); i++) {
    const kernel::elf::Elf32SectionHeader* header =
        (kernel::elf::Elf32SectionHeader*) (elf_sec->addr + sizeof(kernel::elf::Elf32SectionHeader) * i);
    if (header->type != uint32(kernel::elf::SectionType::STRTAB)) {
      continue;
    }

    shell->WriteLine("ELF STRING TABLE %d", i);
    shell->WriteLine("  name:   %d    entsize:    %d", header->name, header->entsize);
    shell->WriteLine("  type:   %d    addralign:  %d", header->type, header->addralign);
    shell->WriteLine("  flags: %d     info:       %d", header->flags, header->info);
    shell->WriteLine("  addr: %h   link:       %d", header->addr, header->link);
    shell->WriteLine("  offset: %d   size:   %d", header->offset, header->size);

    shell->WriteLine("Entries:");
    uint32 index = 0;
    const char* entry = kernel::elf::GetStringTableEntry(header->addr, header->size, index);
    while (entry != nullptr) {
      shell->WriteLine("  %d: %s", index, entry);
      index++;
      entry = kernel::elf::GetStringTableEntry(header->addr, header->size, index);
    }
  }
}

const ShellCommand* GetShellCommand(const char* command) {
  for (size i = 0; i < kNumCommands; i++) {
    if (klib::equal(commands[i].command, command)) {
      return &commands[i];
    }
  }
  return nullptr;
}

}  // anonymous namespace

namespace shell {

ShellStream::ShellStream(const hal::Region region, hal::Offset offset) :
  region_(region), offset_(offset) {}

void ShellStream::Print(char c) {
  // HAX for debugging
  klib::Debug::LogChar(c);

  if (c != '\n') {
    TextUI::SetChar(offset_.x, offset_.y, c);

  // Deal with scrolling.
    offset_.x++;
    if (offset_.x >= region_.offset.x + region_.width) {
      offset_.x = region_.offset.x;
      offset_.y++;
    }
    if (offset_.y >= region_.offset.y + region_.height) {
      TextUI::Scroll(region_);
      offset_.y--;
    }
  }

  if (c == '\n') {
    offset_.x = region_.offset.x;
    offset_.y++;
    if (offset_.y >= region_.offset.y + region_.height) {
      TextUI::Scroll(region_);
      offset_.y--;
    }    
  }
}

hal::Offset ShellStream::Offset() {
  return offset_;
}

void Run() {
  InitializeChrome();
  hal::Region shell_region(0, 1, 80, 24);

  uint8 current_command_line = 1;
  
  const size kMaxCommandLength = 79;  // +1 for null.
  size current_command_idx = 0;
  char current_command[kMaxCommandLength];

  while (true) {
    // Scroll until the command is on the last line.
    while (current_command_line >= 25) {
      TextUI::Scroll(shell_region);
      current_command_line--;
    }

    // Start new command.
    current_command_idx = 0;
    current_command[0] = '\0';

    TextUI::SetChar(0, current_command_line, '$');
    TextUI::SetCursor(2, current_command_line);

    // Read in keystrokes.
    while (true) {
      KeyPress key_press = hal::Keyboard::GetKeypress();
      if (klib::equal(key_press.key.name, "Enter")) {
	break;
      }

      if (klib::equal(key_press.key.name, "Backspace")) {
	if (current_command_idx <= 0) {
	  continue;
	}
	current_command_idx--;
	current_command[current_command_idx] = ' ';
	TextUI::SetChar(current_command_idx + 2, current_command_line, ' ');
	TextUI::SetCursor(current_command_idx + 2, current_command_line);
	continue;
      }

      if (key_press.key.c) {
	TextUI::SetChar(current_command_idx + 2, current_command_line,
			key_press.key.c);

	current_command[current_command_idx] = key_press.key.c;
	current_command_idx++;
	current_command[current_command_idx] = '\0';
	if (current_command_idx >= kMaxCommandLength - 1) {
	  current_command_idx--;
	}

	TextUI::SetCursor(current_command_idx + 2, current_command_line);

	continue;
      }
      // TODO(chrsmith): Delete, and arrow.
    }

    // Process the command.
    current_command_line++;
    while (current_command_line >= 25) {
      TextUI::Scroll(shell_region);
      current_command_line--;
    }

    TextUI::ShowCursor(false);
    hal::Offset shell_offset(0, current_command_line);
    ShellStream stream(shell_region, shell_offset);

    const ShellCommand* command = GetShellCommand(current_command);
    if (klib::equal(current_command, "exit")) {
      return;
    } else if (klib::equal(current_command, "help")) {
      stream.WriteLine("Known Commands:");
      for (size i = 0; i < kNumCommands; i++) {
	stream.WriteLine("  %s", commands[i].command);
      }
    } else if (command == nullptr) {
      stream.WriteLine("Error: Command not found.");
    } else {
      command->func(&stream);
    }

    TextUI::ShowCursor(true);

    // Move the cursor, etc. to where the shell stream finished off.
    hal::Offset offset = stream.Offset();
    current_command_line = offset.y + 1;
  }
}

}  // namespace shell
