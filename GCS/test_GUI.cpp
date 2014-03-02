#include <gtk/gtk.h>

static void callback(GtkWidget *widget, gpointer data)
{
	g_print("Hello again - %s was presssed\n",(gchar*) data);
}

static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	gtk_main_quit();
	return FALSE;
}

int main(int argc, char **argv)
{
	GtkWidget *window;
	GtkWidget *button;
	GtkWidget *box1;
	GtkWidget *table = gtk_table_new(2,2,TRUE);

	gtk_init(&argc,&argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Hello buttons!");
	g_signal_connect(window, "delete_event", G_CALLBACK(delete_event),NULL);
	gtk_container_set_border_width(GTK_CONTAINER(window),10);

	box1 = gtk_vbox_new(FALSE,0);
	//adding box1 into window 
	//gtk_container_add(GTK_CONTAINER(window), box1);
	gtk_container_add(GTK_CONTAINER(window),table);

	button = gtk_button_new_with_label("Button 1");
	g_signal_connect(button,"clicked",G_CALLBACK(callback),(gpointer) "button 1");
	//adding button 1 into box1
	//gtk_box_pack_start(GTK_BOX(box1),button,TRUE,TRUE,0);
	gtk_table_attach_defaults(GTK_TABLE(table),button,0,1,0,1);	
	gtk_widget_show(button);

	button = gtk_button_new_with_label("Button 2");
	g_signal_connect(button,"clicked",G_CALLBACK(callback),(gpointer) "button 2");
	//adding button 2 into box1
	//gtk_box_pack_start(GTK_BOX(box1),button,TRUE,TRUE,0);
	gtk_table_attach_defaults(GTK_TABLE(table),button,1,2,0,1);
	gtk_widget_show(button);

	GtkWidget *frame = gtk_frame_new("Quad status");
	GtkWidget *label = gtk_label_new("state: ");
	gtk_container_add(GTK_CONTAINER(frame),label);
	//adding frame into box1
	//gtk_box_pack_start(GTK_BOX(box1), frame,FALSE,FALSE,0);
	gtk_table_attach_defaults(GTK_TABLE(table),frame,0,1,1,2);

	gtk_widget_show(frame);
	gtk_widget_show(label);
	//show box1
	//gtk_widget_show(box1);
	
	gtk_widget_show(table);
	gtk_widget_show(window);

	gtk_main();
	return 0;	
}
