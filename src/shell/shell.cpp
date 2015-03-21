#include "shell/shell.h"

#include "hal/keyboard.h"
#include "hal/text_ui.h"
#include "kernel/memory.h"
#include "klib/types.h"
#include "klib/strings.h"

using hal::Color;
using hal::TextUI;
using hal::Keyboard::KeyPress;

namespace {

// Initialize shell chrome.
void InitializeChrome();
// Show a map of the machine's memory.
void ShowMemoryMap(shell::ShellStream* shell);

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
  shell->Write("Memory Map #%d", 1);
  shell->Write("Memory Map #%d", 2);
  shell->Write("Memory Map #%d", 3);
  shell->Write("Memory Map #%d" ,4);
  shell->Write("Memory Map #%d", 5);
  /*
  const kernel::grub::multiboot_info_t* multiboot_info =
    kernel::GetMultibootInfo();
  kernel::grub::multiboot_memory_map_t* mmap =
    dynamic_cast<kernel::grub::multiboot_memory_map*>(
      multiboot_info->mmap_addr);

  size map_idx = 0;
  while (((uint32) mmap) < mbt->mmap_addr + mbt->mmap_length) {
    shell->Write("Memory Map [%d][%p] : size %d, type %d", map_idx, (uint32) mmap,
		 mmap->size, mmap->type);
    shell->Write("  address %d / %d",
		 mmap->base_addr_low, mmap->base_addr_high);
    shell->Write("  length  %d / %d", mmap->length_low, mmap->length_high);

    mmap = dynamic_cast<multiboot_memory_map_t*>(
        (uint32) mmap + mmap->size + sizeof(uint32));
    map_idx++;
  }
  */
}

}  // anonymous namespace

namespace shell {

ShellStream::ShellStream(const hal::Region region) : region_(region) {}

void ShellStream::Print(char c) {
  // Write the character.
  // Honor wrap around.
  // Scroll the region as needed.
  if (c > '3') return;
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
    TextUI::Print("Executing '%s'", 0, current_command_line, current_command);

    current_command_line++;  // So we start on the next line.
  }
}

}  // namespace shell
