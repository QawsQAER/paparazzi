#include "GUI.h"


GtkWidget* GUI::window = NULL;
GtkWidget* GUI::button = NULL;
GtkWidget* GUI::box1 = NULL;
GtkWidget* GUI::table = NULL;
GtkWidget* GUI::label = NULL;


GUI::GUI(int argc, char** argv)
{
	#ifdef QUAD_NB
	printf("Quadcopter number is %d\n",QUAD_NB);
	#endif
	table = gtk_table_new(2,2,TRUE);
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
	printf("connecting button\n");
	g_signal_connect(button,"clicked",G_CALLBACK(callback),(gpointer) "button 1");
	printf("connecting button done\n");

	//adding button 1 into box1
	//gtk_box_pack_start(GTK_BOX(box1),button,TRUE,TRUE,0);
	gtk_table_attach_defaults(GTK_TABLE(table),button,0,1,0,1);
	printf("showing button\n");
	gtk_widget_show(button);
	printf("showing button done\n");
	button = gtk_button_new_with_label("Button 2");
	g_signal_connect(button,"clicked",G_CALLBACK(callback),(gpointer) "button 2");
	//adding button 2 into box1
	//gtk_box_pack_start(GTK_BOX(box1),button,TRUE,TRUE,0);
	gtk_table_attach_defaults(GTK_TABLE(table),button,1,2,0,1);
	gtk_widget_show(button);


	quad_status_frame[0] = GUI_quad_status_frame(1);
	printf("creating new quad_status_frame done\n");
	gtk_table_attach_defaults(GTK_TABLE(table),quad_status_frame[0].frame,0,1,1,2);

	gtk_widget_show(quad_status_frame[0].frame);

	gtk_widget_show(table);
	gtk_widget_show(window);
}

GUI::~GUI()
{

}

struct GUI_quad_status_frame GUI::GUI_quad_status_frame(uint8_t AC_ID)
{
			struct GUI_quad_status_frame tmp;
			tmp.frame = gtk_frame_new("Quad_status 1");
			tmp.box = gtk_vbox_new(FALSE,0);
			tmp.label_ned_x = gtk_label_new("ned x: 0");
			tmp.label_ned_y = gtk_label_new("ned y: 0");
			tmp.label_ned_z = gtk_label_new("ned z: 0");
			tmp.label_ecef_x = gtk_label_new("ecef x: 0");
			tmp.label_ecef_y = gtk_label_new("ecef y: 0");
			tmp.label_ecef_z = gtk_label_new("ecef z: 0");
			tmp.label_pacc = gtk_label_new("pacc : 0");
			tmp.label_state = gtk_label_new("state: 0");

			gtk_box_pack_start(GTK_BOX(tmp.box),tmp.label_ned_x,TRUE,TRUE,0);
			gtk_box_pack_start(GTK_BOX(tmp.box),tmp.label_ned_y,TRUE,TRUE,0);
			gtk_box_pack_start(GTK_BOX(tmp.box),tmp.label_ned_z,TRUE,TRUE,0);
			gtk_box_pack_start(GTK_BOX(tmp.box),tmp.label_ecef_x,TRUE,TRUE,0);
			gtk_box_pack_start(GTK_BOX(tmp.box),tmp.label_ecef_y,TRUE,TRUE,0);
			gtk_box_pack_start(GTK_BOX(tmp.box),tmp.label_ecef_z,TRUE,TRUE,0);
			gtk_box_pack_start(GTK_BOX(tmp.box),tmp.label_pacc,TRUE,TRUE,0);
			gtk_box_pack_start(GTK_BOX(tmp.box),tmp.label_state,TRUE,TRUE,0);

			gtk_container_add(GTK_CONTAINER(tmp.frame),tmp.box);

			gtk_widget_show(tmp.label_ned_x);
			gtk_widget_show(tmp.label_ned_y);
			gtk_widget_show(tmp.label_ned_z);
			gtk_widget_show(tmp.label_ecef_x);
			gtk_widget_show(tmp.label_ecef_y);
			gtk_widget_show(tmp.label_ecef_z);
			gtk_widget_show(tmp.label_pacc);
			gtk_widget_show(tmp.label_state);
			gtk_widget_show(tmp.box);

			return tmp;
}

void GUI::GUI_main()
{
	gtk_main();
}
gboolean GUI::delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	gtk_main_quit();
	return FALSE;
}

void GUI::callback(GtkWidget *widget, gpointer data)
{
	g_print("Hello again - %s was presssed\n",(gchar*) data);
	gtk_label_set_text(GTK_LABEL(label),(gchar*) data);	
}

