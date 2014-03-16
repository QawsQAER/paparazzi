#include "GUI.h"


GtkWidget* GUI::window = NULL;
GtkWidget* GUI::button = NULL;
GtkWidget* GUI::table = NULL;
GtkWidget* GUI::label = NULL;
struct GUI_quad_status_frame GUI::quad_status_frame[QUAD_NB + 1];
struct GUI_quad_control_panel GUI::quad_control_panel;


GUI::GUI(int argc, char** argv)
{
	#ifdef QUAD_NB
	printf("Quadcopter number is %d\n",QUAD_NB);
	#endif

	table = gtk_table_new(2,QUAD_NB + 1,TRUE);
	gtk_init(&argc,&argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Quadcopter Ground Control Center");
	g_signal_connect(window, "delete_event", G_CALLBACK(delete_event),NULL);
	gtk_container_set_border_width(GTK_CONTAINER(window),10);
	gtk_container_add(GTK_CONTAINER(window),table);
/*
	button = gtk_button_new_with_label("Button 1");
	printf("connecting button\n");
	g_signal_connect(button,"clicked",G_CALLBACK(callback),(gpointer) "button 1");
	printf("connecting button done\n");

	gtk_table_attach_defaults(GTK_TABLE(table),button,0,1,0,1);
	printf("showing button\n");
	gtk_widget_show(button);
	printf("showing button done\n");
*/
	quad_control_panel = GUI_generate_quad_control_panel();
	GUI_show_quad_control_panel();

	gtk_table_attach_defaults(GTK_TABLE(table),quad_control_panel.frame,0,QUAD_NB + 1,0,1);
	for(uint8_t count_ac = 1;count_ac < QUAD_NB + 1;count_ac++)
	{
		quad_status_frame[count_ac] = GUI_generate_quad_status_frame(count_ac);
		GUI_show_quad_status_frame(count_ac);

		gtk_table_attach_defaults(GTK_TABLE(table),quad_status_frame[count_ac].frame,count_ac - 1,count_ac,1,2);
		//gtk_widget_show(quad_status_frame[count_ac].frame);
		printf("creating new quad_status_frame done\n");
	}

	gtk_widget_show(table);
	gtk_widget_show(window);
}

GUI::~GUI()
{

}

struct GUI_quad_status_frame GUI::GUI_generate_quad_status_frame(uint8_t AC_ID)
{
	struct GUI_quad_status_frame tmp;
	char buffer[32];
	sprintf(buffer,"Quadcopter %d status",AC_ID);
	tmp.frame = gtk_frame_new(buffer);
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

	return tmp;
}

void GUI::GUI_show_quad_status_frame(uint8_t AC_ID)
{
	gtk_widget_show(quad_status_frame[AC_ID].label_ned_x);
	gtk_widget_show(quad_status_frame[AC_ID].label_ned_y);
	gtk_widget_show(quad_status_frame[AC_ID].label_ned_z);
	gtk_widget_show(quad_status_frame[AC_ID].label_ecef_x);
	gtk_widget_show(quad_status_frame[AC_ID].label_ecef_y);
	gtk_widget_show(quad_status_frame[AC_ID].label_ecef_z);
	gtk_widget_show(quad_status_frame[AC_ID].label_pacc);
	gtk_widget_show(quad_status_frame[AC_ID].label_state);
	gtk_widget_show(quad_status_frame[AC_ID].box);
	gtk_widget_show(quad_status_frame[AC_ID].frame);
	return ;
}

struct GUI_quad_control_panel GUI::GUI_generate_quad_control_panel()
{
	struct GUI_quad_control_panel tmp;
	
	tmp.frame = gtk_frame_new("Quadcopters Control");
	tmp.box = gtk_vbox_new(FALSE,0);
	tmp.button_init = gtk_button_new_with_label("init");
	tmp.button_start_engine = gtk_button_new_with_label("start engine");
	tmp.button_takeoff = gtk_button_new_with_label("takeoff");
	tmp.button_landhere = gtk_button_new_with_label("landhere");
	
	gtk_box_pack_start(GTK_BOX(tmp.box),tmp.button_init,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(tmp.box),tmp.button_start_engine,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(tmp.box),tmp.button_takeoff,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(tmp.box),tmp.button_landhere,TRUE,TRUE,0);

	gtk_container_add(GTK_CONTAINER(tmp.frame),tmp.box);
	
	return tmp;
}

void GUI::GUI_show_quad_control_panel()
{
	gtk_widget_show(quad_control_panel.button_init);
	gtk_widget_show(quad_control_panel.button_start_engine);
	gtk_widget_show(quad_control_panel.button_takeoff);
	gtk_widget_show(quad_control_panel.button_landhere);
	gtk_widget_show(quad_control_panel.box);
	gtk_widget_show(quad_control_panel.frame);

	return ;
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

void GUI::button_add_event_listener(GtkWidget *button, void *fun())
{
	g_signal_connect(button,"clicked",G_CALLBACK(fun),NULL);
	return ;
}
