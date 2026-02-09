/*
 * Advanced Text Editor in C with GTK
 * 
 * Features:
 * - Create, open, edit, and save text files
 * - Font customization (style and size)
 * - Menu bar with file operations
 * - Scrollable text area
 * - Dynamic CSS styling
 * - Proper memory management
 * - User-friendly dialogs
 * - Keyboard accessibility
 *
 * Authors: Naik Vedant Vaibhav (23BCE5031), Bhavansh Goyal (23BCE5032)
 */

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

// Global application structure
typedef struct {
    GtkWidget *window;
    GtkWidget *text_view;
    GtkTextBuffer *text_buffer;
    GtkWidget *scrolled_window;
    gchar *current_filename;
    gboolean modified;
    GtkCssProvider *css_provider;
} TextEditor;

// Global pointer for signal handling
static TextEditor *global_editor = NULL;

// Function prototypes
static void activate(GtkApplication *app, gpointer user_data);
static void setup_ui(TextEditor *editor, GtkApplication *app);
static void setup_menu_bar(TextEditor *editor, GtkWidget *vbox);
static void on_new_file(GtkWidget *widget, gpointer data);
static void on_open_file(GtkWidget *widget, gpointer data);
static void on_save_file(GtkWidget *widget, gpointer data);
static void on_save_as_file(GtkWidget *widget, gpointer data);
static void on_quit(GtkWidget *widget, gpointer data);
static void on_font_selection(GtkWidget *widget, gpointer data);
static void on_about(GtkWidget *widget, gpointer data);
static void on_text_changed(GtkTextBuffer *buffer, gpointer data);
static gboolean on_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);
static void apply_css_styling(TextEditor *editor);
static void cleanup_editor(TextEditor *editor);
static void signal_handler(int signum);
static gboolean save_file_internal(TextEditor *editor, const gchar *filename);
static gboolean prompt_save_changes(TextEditor *editor);

// Main function
int main(int argc, char *argv[]) {
    GtkApplication *app;
    int status;

    // Set up signal handlers for clean exit
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Create GTK application
    app = gtk_application_new("com.texteditor.advanced", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    
    status = g_application_run(G_APPLICATION(app), argc, argv);
    
    g_object_unref(app);
    return status;
}

// Application activation callback
static void activate(GtkApplication *app, gpointer user_data) {
    TextEditor *editor;

    // Allocate memory for editor structure
    editor = (TextEditor *)calloc(1, sizeof(TextEditor));
    if (!editor) {
        g_critical("Failed to allocate memory for editor");
        return;
    }

    global_editor = editor;
    editor->current_filename = NULL;
    editor->modified = FALSE;

    // Set up the UI
    setup_ui(editor, app);

    // Apply CSS styling
    apply_css_styling(editor);

    // Show the window
    gtk_widget_show_all(editor->window);
}

// Set up the user interface
static void setup_ui(TextEditor *editor, GtkApplication *app) {
    GtkWidget *vbox;

    // Create main window
    editor->window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(editor->window), "Advanced Text Editor");
    gtk_window_set_default_size(GTK_WINDOW(editor->window), 800, 600);
    g_signal_connect(editor->window, "delete-event", G_CALLBACK(on_delete_event), editor);

    // Create vertical box container
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(editor->window), vbox);

    // Set up menu bar
    setup_menu_bar(editor, vbox);

    // Create scrolled window for text view
    editor->scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(editor->scrolled_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(vbox), editor->scrolled_window, TRUE, TRUE, 0);

    // Create text view and buffer
    editor->text_buffer = gtk_text_buffer_new(NULL);
    editor->text_view = gtk_text_view_new_with_buffer(editor->text_buffer);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(editor->text_view), GTK_WRAP_WORD_CHAR);
    gtk_widget_set_name(editor->text_view, "text-view");
    
    // Connect text changed signal
    g_signal_connect(editor->text_buffer, "changed", G_CALLBACK(on_text_changed), editor);

    // Add text view to scrolled window
    gtk_container_add(GTK_CONTAINER(editor->scrolled_window), editor->text_view);
}

// Set up the menu bar
static void setup_menu_bar(TextEditor *editor, GtkWidget *vbox) {
    GtkWidget *menu_bar;
    GtkWidget *file_menu, *edit_menu, *view_menu, *help_menu;
    GtkWidget *file_item, *edit_item, *view_item, *help_item;
    GtkWidget *new_item, *open_item, *save_item, *save_as_item, *quit_item;
    GtkWidget *font_item, *about_item;

    // Create menu bar
    menu_bar = gtk_menu_bar_new();
    gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);

    // File menu
    file_menu = gtk_menu_new();
    file_item = gtk_menu_item_new_with_mnemonic("_File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_item);

    new_item = gtk_menu_item_new_with_mnemonic("_New");
    g_signal_connect(new_item, "activate", G_CALLBACK(on_new_file), editor);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), new_item);

    open_item = gtk_menu_item_new_with_mnemonic("_Open");
    g_signal_connect(open_item, "activate", G_CALLBACK(on_open_file), editor);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), open_item);

    save_item = gtk_menu_item_new_with_mnemonic("_Save");
    g_signal_connect(save_item, "activate", G_CALLBACK(on_save_file), editor);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_item);

    save_as_item = gtk_menu_item_new_with_mnemonic("Save _As");
    g_signal_connect(save_as_item, "activate", G_CALLBACK(on_save_as_file), editor);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_as_item);

    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), gtk_separator_menu_item_new());

    quit_item = gtk_menu_item_new_with_mnemonic("_Quit");
    g_signal_connect(quit_item, "activate", G_CALLBACK(on_quit), editor);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit_item);

    // Edit menu
    edit_menu = gtk_menu_new();
    edit_item = gtk_menu_item_new_with_mnemonic("_Edit");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_item), edit_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), edit_item);

    // View menu
    view_menu = gtk_menu_new();
    view_item = gtk_menu_item_new_with_mnemonic("_View");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_item), view_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), view_item);

    font_item = gtk_menu_item_new_with_mnemonic("Select _Font");
    g_signal_connect(font_item, "activate", G_CALLBACK(on_font_selection), editor);
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), font_item);

    // Help menu
    help_menu = gtk_menu_new();
    help_item = gtk_menu_item_new_with_mnemonic("_Help");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(help_item), help_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), help_item);

    about_item = gtk_menu_item_new_with_mnemonic("_About");
    g_signal_connect(about_item, "activate", G_CALLBACK(on_about), editor);
    gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), about_item);
}

// New file callback
static void on_new_file(GtkWidget *widget, gpointer data) {
    TextEditor *editor = (TextEditor *)data;

    if (editor->modified && !prompt_save_changes(editor)) {
        return;
    }

    gtk_text_buffer_set_text(editor->text_buffer, "", -1);
    
    if (editor->current_filename) {
        g_free(editor->current_filename);
        editor->current_filename = NULL;
    }
    
    editor->modified = FALSE;
    gtk_window_set_title(GTK_WINDOW(editor->window), "Advanced Text Editor - Untitled");
}

// Open file callback
static void on_open_file(GtkWidget *widget, gpointer data) {
    TextEditor *editor = (TextEditor *)data;
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    if (editor->modified && !prompt_save_changes(editor)) {
        return;
    }

    dialog = gtk_file_chooser_dialog_new("Open File",
                                        GTK_WINDOW(editor->window),
                                        action,
                                        "_Cancel", GTK_RESPONSE_CANCEL,
                                        "_Open", GTK_RESPONSE_ACCEPT,
                                        NULL);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    
    if (res == GTK_RESPONSE_ACCEPT) {
        gchar *filename;
        FILE *file;
        gchar *content;
        long file_size;

        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        
        file = fopen(filename, "r");
        if (file) {
            // Get file size
            fseek(file, 0, SEEK_END);
            file_size = ftell(file);
            fseek(file, 0, SEEK_SET);

            // Allocate memory and read file
            content = (gchar *)malloc(file_size + 1);
            if (content) {
                fread(content, 1, file_size, file);
                content[file_size] = '\0';

                gtk_text_buffer_set_text(editor->text_buffer, content, -1);
                free(content);

                if (editor->current_filename) {
                    g_free(editor->current_filename);
                }
                editor->current_filename = g_strdup(filename);
                editor->modified = FALSE;

                gchar *title = g_strdup_printf("Advanced Text Editor - %s", 
                                              g_path_get_basename(filename));
                gtk_window_set_title(GTK_WINDOW(editor->window), title);
                g_free(title);
            }
            fclose(file);
        } else {
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(editor->window),
                                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                                             GTK_MESSAGE_ERROR,
                                                             GTK_BUTTONS_CLOSE,
                                                             "Failed to open file: %s", filename);
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
        }
        
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

// Save file callback
static void on_save_file(GtkWidget *widget, gpointer data) {
    TextEditor *editor = (TextEditor *)data;

    if (editor->current_filename) {
        save_file_internal(editor, editor->current_filename);
    } else {
        on_save_as_file(widget, data);
    }
}

// Save as file callback
static void on_save_as_file(GtkWidget *widget, gpointer data) {
    TextEditor *editor = (TextEditor *)data;
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Save File",
                                        GTK_WINDOW(editor->window),
                                        action,
                                        "_Cancel", GTK_RESPONSE_CANCEL,
                                        "_Save", GTK_RESPONSE_ACCEPT,
                                        NULL);

    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);

    if (editor->current_filename) {
        gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), editor->current_filename);
    }

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    
    if (res == GTK_RESPONSE_ACCEPT) {
        gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        
        if (save_file_internal(editor, filename)) {
            if (editor->current_filename) {
                g_free(editor->current_filename);
            }
            editor->current_filename = g_strdup(filename);
            
            gchar *title = g_strdup_printf("Advanced Text Editor - %s", 
                                          g_path_get_basename(filename));
            gtk_window_set_title(GTK_WINDOW(editor->window), title);
            g_free(title);
        }
        
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

// Internal save file function
static gboolean save_file_internal(TextEditor *editor, const gchar *filename) {
    FILE *file;
    GtkTextIter start, end;
    gchar *text;
    gboolean success = FALSE;

    file = fopen(filename, "w");
    if (file) {
        gtk_text_buffer_get_bounds(editor->text_buffer, &start, &end);
        text = gtk_text_buffer_get_text(editor->text_buffer, &start, &end, FALSE);
        
        if (fputs(text, file) != EOF) {
            success = TRUE;
            editor->modified = FALSE;
        }
        
        g_free(text);
        fclose(file);
    } else {
        GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(editor->window),
                                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                                         GTK_MESSAGE_ERROR,
                                                         GTK_BUTTONS_CLOSE,
                                                         "Failed to save file: %s", filename);
        gtk_dialog_run(GTK_DIALOG(error_dialog));
        gtk_widget_destroy(error_dialog);
    }

    return success;
}

// Quit callback
static void on_quit(GtkWidget *widget, gpointer data) {
    TextEditor *editor = (TextEditor *)data;

    if (editor->modified && !prompt_save_changes(editor)) {
        return;
    }

    cleanup_editor(editor);
    gtk_main_quit();
}

// Font selection callback
static void on_font_selection(GtkWidget *widget, gpointer data) {
    TextEditor *editor = (TextEditor *)data;
    GtkWidget *font_dialog;
    gint response;

    font_dialog = gtk_font_chooser_dialog_new("Select Font", GTK_WINDOW(editor->window));

    response = gtk_dialog_run(GTK_DIALOG(font_dialog));

    if (response == GTK_RESPONSE_OK) {
        PangoFontDescription *font_desc;
        gchar *font_name = gtk_font_chooser_get_font(GTK_FONT_CHOOSER(font_dialog));
        
        font_desc = pango_font_description_from_string(font_name);
        gtk_widget_override_font(editor->text_view, font_desc);
        
        pango_font_description_free(font_desc);
        g_free(font_name);
    }

    gtk_widget_destroy(font_dialog);
}

// About dialog callback
static void on_about(GtkWidget *widget, gpointer data) {
    TextEditor *editor = (TextEditor *)data;
    GtkWidget *dialog;

    dialog = gtk_message_dialog_new(GTK_WINDOW(editor->window),
                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_INFO,
                                    GTK_BUTTONS_OK,
                                    "Advanced Text Editor in C with GTK");
    
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
        "A feature-rich text editor with:\n"
        "• File operations (New, Open, Save)\n"
        "• Font customization\n"
        "• Scrollable text area\n"
        "• Dynamic CSS styling\n\n"
        "Made by:\n"
        "Naik Vedant Vaibhav (23BCE5031)\n"
        "Bhavansh Goyal (23BCE5032)");

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Text changed callback
static void on_text_changed(GtkTextBuffer *buffer, gpointer data) {
    TextEditor *editor = (TextEditor *)data;
    editor->modified = TRUE;
}

// Window delete event callback
static gboolean on_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data) {
    TextEditor *editor = (TextEditor *)data;

    if (editor->modified && !prompt_save_changes(editor)) {
        return TRUE; // Don't close window
    }

    cleanup_editor(editor);
    return FALSE; // Allow window to close
}

// Prompt to save changes
static gboolean prompt_save_changes(TextEditor *editor) {
    GtkWidget *dialog;
    gint response;

    dialog = gtk_message_dialog_new(GTK_WINDOW(editor->window),
                                    GTK_DIALOG_MODAL,
                                    GTK_MESSAGE_QUESTION,
                                    GTK_BUTTONS_NONE,
                                    "Save changes before closing?");
    
    gtk_dialog_add_buttons(GTK_DIALOG(dialog),
                          "_Don't Save", GTK_RESPONSE_NO,
                          "_Cancel", GTK_RESPONSE_CANCEL,
                          "_Save", GTK_RESPONSE_YES,
                          NULL);

    response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    if (response == GTK_RESPONSE_YES) {
        on_save_file(NULL, editor);
        return TRUE;
    } else if (response == GTK_RESPONSE_NO) {
        return TRUE;
    }

    return FALSE; // Cancel
}

// Apply CSS styling
static void apply_css_styling(TextEditor *editor) {
    GdkScreen *screen;
    const gchar *css_data = 
        "#text-view {"
        "  background-color: #ffffff;"
        "  color: #000000;"
        "  padding: 10px;"
        "}"
        "window {"
        "  background-color: #f0f0f0;"
        "}";

    editor->css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(editor->css_provider, css_data, -1, NULL);

    screen = gdk_screen_get_default();
    gtk_style_context_add_provider_for_screen(screen,
                                              GTK_STYLE_PROVIDER(editor->css_provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

// Clean up editor resources
static void cleanup_editor(TextEditor *editor) {
    if (editor) {
        if (editor->current_filename) {
            g_free(editor->current_filename);
            editor->current_filename = NULL;
        }
        
        if (editor->css_provider) {
            g_object_unref(editor->css_provider);
            editor->css_provider = NULL;
        }
        
        free(editor);
        global_editor = NULL;
    }
}

// Signal handler for clean exit
static void signal_handler(int signum) {
    g_print("\nReceived signal %d, cleaning up...\n", signum);
    
    if (global_editor) {
        cleanup_editor(global_editor);
    }
    
    exit(0);
}
