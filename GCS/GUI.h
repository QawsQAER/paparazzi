#ifndef _GUI_H
#define _GUI_H
#include <gtk/gtk.h>
#include "GUI_struct.h"
#include "main.h"

class GUI
{
	private:
		static GtkWidget* window;
		static GtkWidget* button;
		static GtkWidget* box1;
		static GtkWidget* table;
		static GtkWidget* label;
		struct GUI_quad_status_frame quad_status_frame[QUAD_NB + 1];
	public:
		GUI(int argc, char**argv);
		~GUI();
		static void GUI_main();
		static void callback(GtkWidget *widget, gpointer data);
		static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);
		static struct GUI_quad_status_frame GUI_quad_status_frame(uint8_t AC_ID);
};

#endif