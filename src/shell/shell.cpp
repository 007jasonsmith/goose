#include "shell/shell.h"

#include "hal/keyboard.h"
#include "hal/text_ui.h"
#include "klib/types.h"
#include "klib/strings.h"

using hal::Color;
using hal::TextUI;
using hal::Keyboard::KeyPress;

namespace {

// Initialize shell chrome.
void InitializeChrome();

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

}  // anonymous namespace

namespace shell {

void Run() {
  InitializeChrome();

  uint8 current_command_line = 1;
  
  const size kMaxCommandLength = 79;  // +1 for null.
  size current_command_idx = 0;
  char current_command[kMaxCommandLength];

  while (true) {
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
      }

      // TODO(chrsmith): Backspace, Delete, and arrow.
    }

    // Process the command.
    current_command_line++;
    TextUI::Print(current_command, 0, current_command_line);
    current_command_line++;
  }
}

}  // namespace shell
#if 0
  multiboot_memory_map_t* mmap = (multiboot_memory_map*) mbt->mmap_addr;
  int map_idx = 0;
  while (((uint32) mmap) < mbt->mmap_addr + mbt->mmap_length) {
    con_writeline(OUTPUT_WIN, "Memory Map [%d][%p] : size %d, type %d", map_idx, mmap, mmap->size, mmap->type);
    con_writeline(OUTPUT_WIN, "  address %d / %d", mmap->base_addr_low, mmap->base_addr_high);
    con_writeline(OUTPUT_WIN, "  length  %d / %d", mmap->length_low, mmap->length_high);

    mmap = (multiboot_memory_map_t*) ( (uint32) mmap + mmap->size + sizeof(uint32) );
    map_idx++;
  }

  con_writeline(DEBUG_WIN, "G[%c]ose", 'o');

  con_writeline(OUTPUT_WIN, "Keyboard echo.");

  char command[256];
  while (true) {
    con_write(OUTPUT_WIN, "> ");
    con_win_readline(OUTPUT_WIN, command, 256);
    con_writeline(OUTPUT_WIN, "Executing '%s'", command);

    if (str_compare(command, "crash")) {
      int denum = 1;
      int result = 0 / (denum - 1);
      con_writeline(OUTPUT_WIN, "Result was %d", result);
    }
    if (str_compare(command, "exit")) {
      break;
    }
  }
#endif
