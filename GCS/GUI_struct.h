#include <gtk/gtk.h>

struct GUI_quad_status_frame
{
	GtkWidget *frame;
	GtkWidget *box;
	GtkWidget *label_ned_x;
	GtkWidget *label_ned_y;
	GtkWidget *label_ned_z;
	GtkWidget *label_ecef_x;
	GtkWidget *label_ecef_y;
	GtkWidget *label_ecef_z;
	GtkWidget *label_pacc;
	GtkWidget *label_state;
};

struct GUI_quad_control_panel
{
	GtkWidget *frame;
	GtkWidget *box;
	GtkWidget *button_init;
	GtkWidget *button_start_engine;
	GtkWidget *button_takeoff;
	GtkWidget *button_landhere;
	GtkWidget *button_killed;
};