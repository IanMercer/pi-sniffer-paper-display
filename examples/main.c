/*
   Paper display for MQTT
   Receives messages over DBUS from sniffer
*/

#define G_LOG_USE_STRUCTURED 1
#include <stdlib.h>     //exit()
#include <signal.h>     //signal()
#include "EPD_2in13_V2.h"
#include <glib.h>
#include <gio/gio.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include <sys/types.h>
#include <string.h>
#include <argp.h>

#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "GUI_BMPfile.h"
#include "ImageData.h"
#include "Debug.h"

double people_local;
double people_in_range;


void  Handler(int signo)
{
    //System Exit
    printf("\r\nHandler:exit\r\n");
    DEV_Module_Exit();
    exit(0);
}


/*****************************************************************************
* | File        :   EPD_2IN13_V2_test.c
* | Author      :   Waveshare team
* | Function    :   2.13inch e-paper(V2) test demo
* | Info        :
*----------------
* |     This version:   V1.0
* | Date        :   2019-06-13
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
//#include "EPD_Test.h"
UBYTE *BlackImage;

int main_paper(void)
{
    // Exception handling:ctrl + c
    signal(SIGINT, Handler);

    printf("EPD_2IN13_V2_test Demo\r\n");
    if(DEV_Module_Init()!=0){
        return -1;
    }

    printf("e-Paper Init and Clear...\r\n");
    EPD_2IN13_V2_Init(EPD_2IN13_V2_FULL);
    EPD_2IN13_V2_Clear();
    DEV_Delay_ms(500);

    //Create a new image cache
    /* you have to edit the startup_stm32fxxx.s file and set a big enough heap size */
    UWORD Imagesize = ((EPD_2IN13_V2_WIDTH % 8 == 0)? (EPD_2IN13_V2_WIDTH / 8 ): (EPD_2IN13_V2_WIDTH / 8 + 1)) * EPD_2IN13_V2_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }
    printf("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_2IN13_V2_WIDTH, EPD_2IN13_V2_HEIGHT, 270, WHITE);
    Paint_SelectImage(BlackImage);
    Paint_SetMirroring(MIRROR_HORIZONTAL); //
    Paint_Clear(WHITE);

#if 1   // show bmp
    printf("show window BMP-----------------\r\n");
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    GUI_ReadBmp("./pic/100x100.bmp", 10, 10);
    EPD_2IN13_V2_Display(BlackImage);
    DEV_Delay_ms(2000);

    printf("show bmp------------------------\r\n");
    Paint_SelectImage(BlackImage);
    GUI_ReadBmp("./pic/2in13-v2.bmp", 0, 0);
    EPD_2IN13_V2_Display(BlackImage);
    DEV_Delay_ms(2000);
#endif


#if 1   //show image for array
    printf("show image for array\r\n");
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    Paint_DrawBitMap(gImage_2in13);

    EPD_2IN13_V2_Display(BlackImage);
    DEV_Delay_ms(2000);
#endif

#if 1   // Drawing on the image
    printf("Drawing\r\n");
    //1.Select Image
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);

    // 2.Drawing on the image
    Paint_DrawPoint(5, 10, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
    Paint_DrawPoint(5, 25, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
    Paint_DrawPoint(5, 40, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
    Paint_DrawPoint(5, 55, BLACK, DOT_PIXEL_4X4, DOT_STYLE_DFT);

    Paint_DrawLine(20, 10, 70, 60, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(70, 10, 20, 60, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawRectangle(20, 10, 70, 60, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(85, 10, 135, 60, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    Paint_DrawLine(45, 15, 45, 55, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(25, 35, 70, 35, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawCircle(45, 35, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawCircle(110, 35, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    Paint_DrawString_EN(140, 15, "waveshare", &Font16, BLACK, WHITE);
    Paint_DrawNum(140, 40, 123456789, &Font16, BLACK, WHITE);

    Paint_DrawString_CN(140, 60, "���abc", &Font12CN, BLACK, WHITE);
    Paint_DrawString_CN(5, 65, "΢ѩ����", &Font24CN, WHITE, BLACK);

    EPD_2IN13_V2_Display(BlackImage);
    DEV_Delay_ms(2000);
#endif

#if 1   //Partial refresh, example shows time
    printf("Partial refresh\r\n");
    EPD_2IN13_V2_Init(EPD_2IN13_V2_FULL);
    EPD_2IN13_V2_DisplayPartBaseImage(BlackImage);

    EPD_2IN13_V2_Init(EPD_2IN13_V2_PART);
    Paint_SelectImage(BlackImage);
    PAINT_TIME sPaint_time;
    sPaint_time.Hour = 12;
    sPaint_time.Min = 34;
    sPaint_time.Sec = 56;
    UBYTE num = 20;
    for (;;) {
        sPaint_time.Sec = sPaint_time.Sec + 1;
        if (sPaint_time.Sec == 60) {
            sPaint_time.Min = sPaint_time.Min + 1;
            sPaint_time.Sec = 0;
            if (sPaint_time.Min == 60) {
                sPaint_time.Hour =  sPaint_time.Hour + 1;
                sPaint_time.Min = 0;
                if (sPaint_time.Hour == 24) {
                    sPaint_time.Hour = 0;
                    sPaint_time.Min = 0;
                    sPaint_time.Sec = 0;
                }
            }
        }
        Paint_ClearWindows(140, 90, 140 + Font20.Width * 7, 90 + Font20.Height, WHITE);
        Paint_DrawTime(140, 90, &sPaint_time, &Font20, WHITE, BLACK);

        num = num - 1;
        if(num == 0) {
            break;
        }
        EPD_2IN13_V2_DisplayPart(BlackImage);
        DEV_Delay_ms(500);//Analog clock 1s
    }

#endif
    printf("Clear...\r\n");

    EPD_2IN13_V2_Init(EPD_2IN13_V2_FULL);
    EPD_2IN13_V2_Clear();
    DEV_Delay_ms(2000);//Analog clock 1s

    printf("Goto Sleep...\r\n");
    EPD_2IN13_V2_Sleep();
    free(BlackImage);
    BlackImage = NULL;
    DEV_Delay_ms(1000);//Analog clock 1s

    // close 5V
    printf("close 5V, Module enters 0 power consumption ...\r\n");
    DEV_Module_Exit();

    return 0;
}




/*
      Connection to DBUS
*/
GDBusConnection *conn;
// Time when service started running (used to print a delta time)
static time_t started;

// Handle Ctrl-c
void int_handler(int);

/*
    people_updated()
*/


static void people_updated(GDBusConnection *conn,
                                         const gchar *sender,
                                         const gchar *path,
                                         const gchar *interface,
                                         const gchar *signal,
                                         GVariant *params,
                                         void *userdata)
{
    (void)conn;
    (void)sender;    // "1.4120"
    (void)path;      // "/org/..."
    (void)interface; // "org...."
    (void)userdata;

    //g_info("people_updated(sender=%s, path=%s, interface=%s)", sender, path, interface);
    //pretty_print("Params", params);

    const gchar *signature = g_variant_get_type_string(params);
    if (strcmp(signature, "(a{sv})") != 0)
    {
        g_warning("Invalid signature for %s: %s != %s", signal, signature, "(a{sv})");
        return;
    }

    // It's wrapped in a tuple
    GVariant *array = g_variant_get_child_value(params, 0);

    const gchar *property_name;
    GVariantIter i;
    GVariant *prop_val;
    g_variant_iter_init(&i, array);    // no need to free this
    while (g_variant_iter_next(&i, "{&sv}", &property_name, &prop_val))
    {
        double d = g_variant_get_double(prop_val);
        g_info("%s = %.2f", property_name, d);
        g_variant_unref(prop_val);

        if (strcmp(property_name, "PeopleClosest") == 0) {
           people_local = d;
        }
        if (strcmp(property_name, "PeopleInRange") == 0) {
           people_in_range = d;
        }
    }

    return;
}

void initialize_display()
{
    if(DEV_Module_Init()!=0){
        return;
    }

    printf("e-Paper Init and Clear...\r\n");
    EPD_2IN13_V2_Init(EPD_2IN13_V2_FULL);
    EPD_2IN13_V2_Clear();
    DEV_Delay_ms(500);

    //Create a new image cache
    /* you have to edit the startup_stm32fxxx.s file and set a big enough heap size */
    UWORD Imagesize = ((EPD_2IN13_V2_WIDTH % 8 == 0)? (EPD_2IN13_V2_WIDTH / 8 ): (EPD_2IN13_V2_WIDTH / 8 + 1)) * EPD_2IN13_V2_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return;
    }
    printf("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_2IN13_V2_WIDTH, EPD_2IN13_V2_HEIGHT, 270, WHITE);
    Paint_SelectImage(BlackImage);
    Paint_SetMirroring(MIRROR_HORIZONTAL); //
    Paint_Clear(WHITE);


//    EPD_2IN13_V2_Init(EPD_2IN13_V2_PART);
//    Paint_SelectImage(BlackImage);

    // 2.Drawing on the image
/*
    Paint_DrawPoint(5, 10, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
    Paint_DrawPoint(5, 25, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
    Paint_DrawPoint(5, 40, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
    Paint_DrawPoint(5, 55, BLACK, DOT_PIXEL_4X4, DOT_STYLE_DFT);

    Paint_DrawLine(20, 10, 70, 60, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(70, 10, 20, 60, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawRectangle(20, 10, 70, 60, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(85, 10, 135, 60, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
*/
    Paint_DrawLine(45, 15, 45, 55, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(25, 35, 70, 35, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawCircle(45, 35, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawCircle(110, 35, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);


    GUI_ReadBmp("./pic/person.bmp", 0, 0);

    Paint_DrawString_EN(100, 15, "Signswift", &Font16, WHITE, BLACK);
    Paint_DrawString_EN(100, 15 + Font20.Height, "Crowd Alert", &Font16, WHITE, BLACK);

    EPD_2IN13_V2_Init(EPD_2IN13_V2_FULL);
    EPD_2IN13_V2_DisplayPartBaseImage(BlackImage);

    EPD_2IN13_V2_Init(EPD_2IN13_V2_PART);
    Paint_SelectImage(BlackImage);
}


int update_display(void* parameters)
{
    g_info("Update display %i x %i + %i", EPD_2IN13_V2_WIDTH, EPD_2IN13_V2_HEIGHT, Font20.Height);

    Paint_ClearWindows(100, 60, EPD_2IN13_V2_HEIGHT-1, 60 + Font20.Height, WHITE);
    Paint_ClearWindows(100, 85, EPD_2IN13_V2_HEIGHT-1, 85 + Font20.Height, WHITE);

    char value_string[64];
    snprintf(value_string, sizeof(value_string), "Here   %.1f", people_local);
    Paint_DrawString_EN(100, 60, value_string, &Font16, WHITE, BLACK);

    snprintf(value_string, sizeof(value_string), "Nearby %.1f", people_in_range);
    Paint_DrawString_EN(100, 85, value_string, &Font16, WHITE, BLACK);

    EPD_2IN13_V2_DisplayPart(BlackImage);
    return TRUE;
}



int update_clock(void *parameters)
{
    (void)parameters;
    g_info("Tick");

//    Paint_SelectImage(BlackImage);
//    GUI_ReadBmp("./pic/2in13-v2.bmp", 0, 0);
//    EPD_2IN13_V2_Display(BlackImage);

    return TRUE;
}


guint prop_changed;
GMainLoop *loop;

int main(int argc, char **argv)
{
    (void)argv;
    //int rc;

    g_info("Starting paper display");

    signal(SIGINT, int_handler);
    signal(SIGTERM, int_handler);

    initialize_display();

    conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, NULL);
    if (conn == NULL)
    {
        g_warning("Not able to get connection to system bus\n");
        return 1;
    }

    // Grab zero time = time when started
    time(&started);

    loop = g_main_loop_new(NULL, FALSE);

    prop_changed = g_dbus_connection_signal_subscribe(conn,
                                                      NULL,
                                                      "com.signswift.sniffer",
                                                      NULL, //"People",
                                                      "/com/signswift/sniffer",
                                                      NULL,
                                                      G_DBUS_SIGNAL_FLAGS_NONE,
                                                      people_updated,
                                                      NULL,
                                                      NULL);

    // Every 5s update people count
    g_timeout_add_seconds(5, update_display, loop);

    // Every 5s update clock
    g_timeout_add_seconds(5, update_clock, loop);

    g_info("Start main loop\n");

    g_main_loop_run(loop);

    g_info("END OF MAIN LOOP RUN\n");

    g_dbus_connection_signal_unsubscribe(conn, prop_changed);
    g_dbus_connection_close_sync(conn, NULL, NULL);
    g_object_unref(conn);
    return 0;
}

void int_handler(int dummy)
{
    g_info("Int handler %i", dummy);

    g_main_loop_quit(loop);
    g_main_loop_unref(loop);

    g_dbus_connection_signal_unsubscribe(conn, prop_changed);
    g_dbus_connection_close_sync(conn, NULL, NULL);
    g_object_unref(conn);

    g_info("Clean exit");

    exit(0);
}
