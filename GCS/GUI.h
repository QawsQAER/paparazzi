#include <gtk/gtk.h>

class GUI
{
	private:
		static GtkWidget* window;
		static GtkWidget* button;
		static GtkWidget* box1;
		static GtkWidget* table;
		static GtkWidget* label;
	public:
		GUI(int argc, char**argv);
		~GUI();
		static void callback(GtkWidget *widget, gpointer data);
		static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);
};

