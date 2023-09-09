#!/bin/sh

GList *some_list, *l;

some_list = g_app_info_get_all(); /*This is where the GList function goes

for (l = some_list; l != NULL; l = l->next)
  {
    gpointer element_data = l->data;
    printf("%s\n", g_app_info_get_display_name(element_data)); /*print out all of the display names of the .desktop files */
  }
return 0;
