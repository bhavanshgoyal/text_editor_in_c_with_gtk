# Advanced Text Editor in C with GTK

A feature-rich text editor built with C and GTK+ 3, providing essential text-editing functionalities with a user-friendly interface.

## Authors
- Naik Vedant Vaibhav (23BCE5031)
- Bhavansh Goyal (23BCE5032)

## Features

### Core Functionality
- **File Operations**: Create new files, open existing files, save and save-as functionality
- **Text Editing**: Full-featured text area with word wrapping
- **Scrollable Interface**: Smooth scrolling for large documents
- **Menu Bar**: Organized menu with File, Edit, View, and Help options

### Customization
- **Font Selection**: Choose custom fonts and sizes using GTK Font Chooser
- **Dynamic CSS Styling**: Customizable appearance with CSS-based theming
- **User-Friendly Dialogs**: Clear and informative message dialogs

### Technical Features
- **Memory Management**: Proper use of malloc/calloc/free to prevent memory leaks
- **Clean Exit Handling**: Signal handlers for graceful shutdown (SIGINT, SIGTERM)
- **Keyboard Accessibility**: Full keyboard navigation support
- **Modified File Tracking**: Prompts to save unsaved changes

## Requirements

### System Requirements
- Linux-based operating system (Ubuntu, Debian, Fedora, Arch, etc.)
- GTK+ 3 development libraries
- GCC compiler
- pkg-config

### Installing Dependencies

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install build-essential libgtk-3-dev pkg-config
```

**Fedora:**
```bash
sudo dnf install gcc gtk3-devel pkg-config
```

**Arch Linux:**
```bash
sudo pacman -S base-devel gtk3 pkg-config
```

## Building the Application

### Using Make
```bash
# Build the application
make

# Run the application
make run

# Clean build files
make clean
```

### Manual Compilation
```bash
gcc `pkg-config --cflags gtk+-3.0` -o text_editor text_editor.c `pkg-config --libs gtk+-3.0`
./text_editor
```

## Usage

### Starting the Editor
```bash
./text_editor
```

### Menu Options

#### File Menu
- **New** (Ctrl+N): Create a new document
- **Open** (Ctrl+O): Open an existing file
- **Save** (Ctrl+S): Save the current file
- **Save As**: Save with a new filename
- **Quit** (Ctrl+Q): Exit the application

#### View Menu
- **Select Font**: Choose custom font and size

#### Help Menu
- **About**: Display information about the application

### Keyboard Shortcuts
- **Ctrl+N**: New file
- **Ctrl+O**: Open file
- **Ctrl+S**: Save file
- **Ctrl+Q**: Quit application

## Code Structure

### Main Components

1. **TextEditor Structure**: Contains all editor state and widgets
   - Window, text view, text buffer
   - Current filename and modification status
   - CSS provider for styling

2. **GTK Application Framework**: Uses GtkApplication for lifecycle management
   - Clean initialization and shutdown
   - Event handling and signal management

3. **Menu System**: Comprehensive menu bar with all operations
   - File operations with dialogs
   - Customization options
   - Help and about information

4. **File Handling**: Robust file I/O operations
   - Standard C file operations (fopen, fread, fwrite)
   - GTK file chooser dialogs
   - Error handling and user feedback

5. **Memory Management**: Careful resource allocation
   - Proper use of malloc/calloc/free
   - Signal handlers for cleanup
   - GTK object reference management

## Memory Safety

The application includes:
- Proper memory allocation and deallocation
- Signal handlers for SIGINT and SIGTERM
- Clean shutdown procedures
- Prevention of memory leaks

To check for memory leaks, you can use Valgrind:
```bash
valgrind --leak-check=full ./text_editor
```

## Customization

### CSS Styling
The application uses GTK CSS for styling. You can modify the `apply_css_styling()` function to change:
- Background colors
- Text colors
- Padding and margins
- Font defaults

### Adding Features
The modular structure makes it easy to add new features:
- Edit menu items (cut, copy, paste)
- Search and replace functionality
- Line numbers
- Syntax highlighting
- Multiple document interface

## Troubleshooting

### GTK+ Not Found
```
Package gtk+-3.0 was not found in the pkg-config search path
```
**Solution**: Install GTK+ 3 development libraries (see Dependencies section)

### Compilation Errors
**Solution**: Ensure you have the complete GTK+ 3 development package installed

### Runtime Errors
**Solution**: Check that GTK+ 3 runtime libraries are properly installed

## Architecture Overview

```
┌─────────────────────────────────────────┐
│          GtkApplication                 │
│         (Application Lifecycle)          │
└──────────────┬──────────────────────────┘
               │
┌──────────────▼──────────────────────────┐
│         GtkWindow (Main Window)         │
├─────────────────────────────────────────┤
│  ┌────────────────────────────────────┐ │
│  │      GtkMenuBar                    │ │
│  │  [File] [Edit] [View] [Help]      │ │
│  └────────────────────────────────────┘ │
│  ┌────────────────────────────────────┐ │
│  │    GtkScrolledWindow               │ │
│  │  ┌──────────────────────────────┐  │ │
│  │  │     GtkTextView              │  │ │
│  │  │   (Text Editing Area)        │  │ │
│  │  │                              │  │ │
│  │  └──────────────────────────────┘  │ │
│  └────────────────────────────────────┘ │
└─────────────────────────────────────────┘
```

## Contributing

Feel free to extend this text editor with additional features:
- Undo/Redo functionality
- Find and Replace
- Line numbering
- Status bar
- Syntax highlighting
- Multiple tabs
- Recent files menu
- Auto-save functionality

## License

This is an educational project created for demonstration purposes.

## Acknowledgments

Built using:
- GTK+ 3 (GIMP Toolkit)
- GLib/GObject
- Pango (text rendering)

## Contact

For questions or suggestions, please contact the authors:
- Naik Vedant Vaibhav (23BCE5031)
- Bhavansh Goyal (23BCE5032)
