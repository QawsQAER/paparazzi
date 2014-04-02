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
	GtkWidget *button_end_negotiate;
	GtkWidget *button_start_engine;
	GtkWidget *button_takeoff;
	GtkWidget *button_landhere;
	GtkWidget *button_takeoff_again;
	GtkWidget *button_swap;
};

struct GUI_quad_flight_control
{
	GtkWidget *frame;
	GtkWidget *box;
	GtkWidget *button_execute;
	GtkWidget *button_go_north;
	GtkWidget *button_go_south;
	GtkWidget *button_go_east;
	GtkWidget *button_go_west;
	GtkWidget *button_takeoff1;
	GtkWidget *button_takeoff2;
	GtkWidget *button_takeoff3;
	GtkWidget *button_takeoff4;
};

struct GUI_GCS_status_frame
{
	GtkWidget *frame;
	GtkWidget *box;
	GtkWidget *label_status;	
};