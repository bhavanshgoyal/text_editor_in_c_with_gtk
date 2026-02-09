/*
 * Extended Text Editor Features
 * 
 * This file demonstrates additional features that can be added:
 * - Undo/Redo functionality
 * - Find and Replace
 * - Line numbers
 * - Status bar
 * 
 * Add these snippets to the main text_editor.c file
 */

// ============================================
// UNDO/REDO FUNCTIONALITY
// ============================================

// Add to TextEditor structure:
/*
typedef struct {
    // ... existing fields ...
    GtkWidget *undo_button;
    GtkWidget *redo_button;
} TextEditor;
*/

// Undo callback
static void on_undo(GtkWidget *widget, gpointer data) {
    TextEditor *editor = (TextEditor *)data;
    
    if (gtk_text_buffer_get_can_undo(editor->text_buffer)) {
        gtk_text_buffer_undo(editor->text_buffer);
    }
}

// Redo callback
static void on_redo(GtkWidget *widget, gpointer data) {
    TextEditor *editor = (TextEditor *)data;
    
    if (gtk_text_buffer_get_can_redo(editor->text_buffer)) {
        gtk_text_buffer_redo(editor->text_buffer);
    }
}

// Enable undo in setup:
/*
static void setup_text_buffer(TextEditor *editor) {
    editor->text_buffer = gtk_text_buffer_new(NULL);
    gtk_text_buffer_set_enable_undo(editor->text_buffer, TRUE);
    gtk_text_buffer_set_max_undo_levels(editor->text_buffer, 100);
}
*/

// ============================================
// FIND AND REPLACE FUNCTIONALITY
// ============================================

typedef struct {
    TextEditor *editor;
    GtkWidget *find_entry;
    GtkWidget *replace_entry;
    GtkTextIter current_match;
} FindReplaceData;

// Find text callback
static void on_find(GtkWidget *widget, gpointer data) {
    TextEditor *editor = (TextEditor *)data;
    GtkWidget *dialog, *content_area;
    GtkWidget *find_entry, *find_button, *close_button;
    GtkWidget *hbox, *label;

    dialog = gtk_dialog_new_with_buttons("Find",
                                        GTK_WINDOW(editor->window),
                                        GTK_DIALOG_MODAL,
                                        "_Close", GTK_RESPONSE_CLOSE,
                                        NULL);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_set_margin_start(GTK_BOX(hbox), 10);
    gtk_box_set_margin_end(GTK_BOX(hbox), 10);
    gtk_box_set_margin_top(GTK_BOX(hbox), 10);
    gtk_box_set_margin_bottom(GTK_BOX(hbox), 10);

    label = gtk_label_new("Find:");
    find_entry = gtk_entry_new();
    find_button = gtk_button_new_with_label("Find Next");

    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), find_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), find_button, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(content_area), hbox);
    gtk_widget_show_all(dialog);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Find next occurrence
static gboolean find_next(TextEditor *editor, const gchar *search_text) {
    GtkTextIter start, end, match_start, match_end;
    gboolean found;

    // Get current cursor position
    GtkTextMark *insert_mark = gtk_text_buffer_get_insert(editor->text_buffer);
    gtk_text_buffer_get_iter_at_mark(editor->text_buffer, &start, insert_mark);
    
    // Get end of buffer
    gtk_text_buffer_get_end_iter(editor->text_buffer, &end);

    // Search forward
    found = gtk_text_iter_forward_search(&start, search_text,
                                        GTK_TEXT_SEARCH_TEXT_ONLY,
                                        &match_start, &match_end, &end);

    if (found) {
        // Select the found text
        gtk_text_buffer_select_range(editor->text_buffer, &match_start, &match_end);
        
        // Scroll to the match
        gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(editor->text_view),
                                    &match_start, 0.0, FALSE, 0.0, 0.0);
        
        return TRUE;
    } else {
        // Wrap around to beginning
        gtk_text_buffer_get_start_iter(editor->text_buffer, &start);
        found = gtk_text_iter_forward_search(&start, search_text,
                                            GTK_TEXT_SEARCH_TEXT_ONLY,
                                            &match_start, &match_end, NULL);
        
        if (found) {
            gtk_text_buffer_select_range(editor->text_buffer, &match_start, &match_end);
            gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(editor->text_view),
                                        &match_start, 0.0, FALSE, 0.0, 0.0);
            return TRUE;
        }
    }

    return FALSE;
}

// Replace text callback
static void on_replace(GtkWidget *widget, gpointer data) {
    TextEditor *editor = (TextEditor *)data;
    GtkWidget *dialog, *content_area;
    GtkWidget *find_entry, *replace_entry;
    GtkWidget *find_button, *replace_button, *replace_all_button;
    GtkWidget *grid, *find_label, *replace_label;

    dialog = gtk_dialog_new_with_buttons("Find and Replace",
                                        GTK_WINDOW(editor->window),
                                        GTK_DIALOG_MODAL,
                                        "_Close", GTK_RESPONSE_CLOSE,
                                        NULL);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_widget_set_margin_start(grid, 10);
    gtk_widget_set_margin_end(grid, 10);
    gtk_widget_set_margin_top(grid, 10);
    gtk_widget_set_margin_bottom(grid, 10);

    find_label = gtk_label_new("Find:");
    replace_label = gtk_label_new("Replace:");
    find_entry = gtk_entry_new();
    replace_entry = gtk_entry_new();
    find_button = gtk_button_new_with_label("Find Next");
    replace_button = gtk_button_new_with_label("Replace");
    replace_all_button = gtk_button_new_with_label("Replace All");

    gtk_grid_attach(GTK_GRID(grid), find_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), find_entry, 1, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), replace_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), replace_entry, 1, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), find_button, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), replace_button, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), replace_all_button, 2, 2, 1, 1);

    gtk_container_add(GTK_CONTAINER(content_area), grid);
    gtk_widget_show_all(dialog);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// ============================================
// LINE NUMBERS
// ============================================

// Enable line numbers
static void on_toggle_line_numbers(GtkWidget *widget, gpointer data) {
    TextEditor *editor = (TextEditor *)data;
    gboolean show_line_numbers;
    
    show_line_numbers = gtk_text_view_get_show_line_numbers(
        GTK_TEXT_VIEW(editor->text_view));
    
    gtk_text_view_set_show_line_numbers(GTK_TEXT_VIEW(editor->text_view),
                                       !show_line_numbers);
}

// ============================================
// STATUS BAR
// ============================================

// Add to TextEditor structure:
/*
typedef struct {
    // ... existing fields ...
    GtkWidget *status_bar;
    guint status_context_id;
} TextEditor;
*/

// Create status bar
static void create_status_bar(TextEditor *editor, GtkWidget *vbox) {
    editor->status_bar = gtk_statusbar_new();
    editor->status_context_id = gtk_statusbar_get_context_id(
        GTK_STATUSBAR(editor->status_bar), "editor-status");
    
    gtk_box_pack_end(GTK_BOX(vbox), editor->status_bar, FALSE, FALSE, 0);
    
    update_status_bar(editor);
}

// Update status bar with cursor position
static void update_status_bar(TextEditor *editor) {
    GtkTextIter iter;
    GtkTextMark *mark;
    gint line, column;
    gchar *status_text;

    mark = gtk_text_buffer_get_insert(editor->text_buffer);
    gtk_text_buffer_get_iter_at_mark(editor->text_buffer, &iter, mark);

    line = gtk_text_iter_get_line(&iter) + 1;
    column = gtk_text_iter_get_line_offset(&iter) + 1;

    status_text = g_strdup_printf("Line %d, Column %d", line, column);
    
    gtk_statusbar_pop(GTK_STATUSBAR(editor->status_bar), 
                     editor->status_context_id);
    gtk_statusbar_push(GTK_STATUSBAR(editor->status_bar),
                      editor->status_context_id, status_text);
    
    g_free(status_text);
}

// Connect to cursor position changed
static void on_cursor_position_changed(GtkTextBuffer *buffer, 
                                       GParamSpec *pspec, 
                                       gpointer data) {
    update_status_bar((TextEditor *)data);
}

// ============================================
// RECENT FILES
// ============================================

// Add to TextEditor structure:
/*
typedef struct {
    // ... existing fields ...
    GtkRecentManager *recent_manager;
    GtkWidget *recent_menu_item;
} TextEditor;
*/

// Add file to recent files
static void add_to_recent_files(TextEditor *editor, const gchar *filename) {
    gchar *uri;
    GtkRecentData *recent_data;

    if (!editor->recent_manager) {
        editor->recent_manager = gtk_recent_manager_get_default();
    }

    uri = g_filename_to_uri(filename, NULL, NULL);
    
    recent_data = g_slice_new(GtkRecentData);
    recent_data->display_name = NULL;
    recent_data->description = NULL;
    recent_data->mime_type = "text/plain";
    recent_data->app_name = g_strdup("Advanced Text Editor");
    recent_data->app_exec = g_strdup_printf("%s %%u", g_get_prgname());
    recent_data->groups = NULL;
    recent_data->is_private = FALSE;

    gtk_recent_manager_add_full(editor->recent_manager, uri, recent_data);

    g_free(uri);
    g_free(recent_data->app_name);
    g_free(recent_data->app_exec);
    g_slice_free(GtkRecentData, recent_data);
}

// ============================================
// AUTO-SAVE
// ============================================

// Auto-save timer callback
static gboolean auto_save_callback(gpointer data) {
    TextEditor *editor = (TextEditor *)data;
    
    if (editor->modified && editor->current_filename) {
        // Save file without showing dialog
        save_file_internal(editor, editor->current_filename);
        
        // Update status bar
        gchar *status = g_strdup_printf("Auto-saved to %s", 
                                       g_path_get_basename(editor->current_filename));
        gtk_statusbar_push(GTK_STATUSBAR(editor->status_bar),
                          editor->status_context_id, status);
        g_free(status);
    }
    
    return TRUE; // Continue timer
}

// Enable auto-save (call during setup)
static void enable_auto_save(TextEditor *editor) {
    // Auto-save every 5 minutes (300000 milliseconds)
    g_timeout_add(300000, auto_save_callback, editor);
}

// ============================================
// WORD COUNT
// ============================================

static void show_word_count(GtkWidget *widget, gpointer data) {
    TextEditor *editor = (TextEditor *)data;
    GtkTextIter start, end;
    gchar *text;
    gint word_count = 0, char_count = 0, line_count = 0;
    GtkWidget *dialog;

    gtk_text_buffer_get_bounds(editor->text_buffer, &start, &end);
    text = gtk_text_buffer_get_text(editor->text_buffer, &start, &end, FALSE);
    
    // Count characters
    char_count = g_utf8_strlen(text, -1);
    
    // Count lines
    line_count = gtk_text_buffer_get_line_count(editor->text_buffer);
    
    // Count words
    gchar **words = g_strsplit_set(text, " \t\n\r", -1);
    for (gint i = 0; words[i] != NULL; i++) {
        if (g_utf8_strlen(words[i], -1) > 0) {
            word_count++;
        }
    }
    g_strfreev(words);
    g_free(text);

    dialog = gtk_message_dialog_new(GTK_WINDOW(editor->window),
                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_INFO,
                                    GTK_BUTTONS_OK,
                                    "Document Statistics");
    
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
        "Characters: %d\nWords: %d\nLines: %d",
        char_count, word_count, line_count);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// ============================================
// INTEGRATION NOTES
// ============================================

/*
To add these features to the main text_editor.c:

1. Add new fields to TextEditor structure
2. Add menu items in setup_menu_bar():
   - Edit menu: Undo, Redo, Find, Replace
   - View menu: Toggle Line Numbers, Word Count
   
3. Connect callbacks in setup_ui()

4. Initialize features:
   - setup_text_buffer() for undo/redo
   - create_status_bar() for status bar
   - enable_auto_save() for auto-save
   
Example menu additions:

// In Edit menu
undo_item = gtk_menu_item_new_with_mnemonic("_Undo");
g_signal_connect(undo_item, "activate", G_CALLBACK(on_undo), editor);
gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), undo_item);

redo_item = gtk_menu_item_new_with_mnemonic("_Redo");
g_signal_connect(redo_item, "activate", G_CALLBACK(on_redo), editor);
gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), redo_item);

find_item = gtk_menu_item_new_with_mnemonic("_Find");
g_signal_connect(find_item, "activate", G_CALLBACK(on_find), editor);
gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), find_item);

replace_item = gtk_menu_item_new_with_mnemonic("_Replace");
g_signal_connect(replace_item, "activate", G_CALLBACK(on_replace), editor);
gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), replace_item);

// In View menu
line_numbers_item = gtk_check_menu_item_new_with_mnemonic("Show _Line Numbers");
g_signal_connect(line_numbers_item, "activate", 
                G_CALLBACK(on_toggle_line_numbers), editor);
gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), line_numbers_item);

word_count_item = gtk_menu_item_new_with_mnemonic("_Word Count");
g_signal_connect(word_count_item, "activate", G_CALLBACK(show_word_count), editor);
gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), word_count_item);
*/
