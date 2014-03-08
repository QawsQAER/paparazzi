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