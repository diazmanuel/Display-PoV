/* MIT License
 *
 * Copyright (c) 2017 spino.tech Guillaume Chereau & Raphael Seghier
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "spino.h"

#if DEFINED(BLUETOOTH_BLUEZ)

#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>

static pthread_t g_thread;
static int g_status = 0;
static int g_socket = 0;

// Keep the last used bt.
static struct {
    const char *name;
    const char *addr;
} g_stored = {0};

// All the listed devices.
static struct device_t {
    char addr[19];
    char name[248];
} g_devices[256] = {};

void bluetooth_init(void)
{
    g_status = SPINO_BT_IDLE;
    // Get all the stored addresses.
    g_stored.name = storage_get("bluetooth_last_name", NULL);
    g_stored.addr = storage_get("bluetooth_last_addr", NULL);
}

static void *thread_func(void *args)
{
    int i, dev_id, sock, max_rsp, num_rsp, len, flags = 0;
    inquiry_info *ii = NULL;

    LOG_D("start");
    dev_id = hci_get_route(NULL);
    sock = hci_open_dev( dev_id );
    if (dev_id < 0 || sock < 0) {
        LOG_W("Error opening BT socket");
        g_status = SPINO_BT_ERROR;
        return NULL;
    }
    len = 8;
    max_rsp = 255;
    flags = IREQ_CACHE_FLUSH;
    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));
    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
    if (num_rsp < 0) {
        perror("Error hci_inquiry");
        LOG_E("HCI inquiry failed: %d", num_rsp);
        g_status = SPINO_BT_ERROR;
        return NULL;
    }
    for (i = 0; i < num_rsp; i++) {
        ba2str(&(ii+i)->bdaddr, g_devices[i].addr);
        if (hci_read_remote_name(sock, &(ii+i)->bdaddr,
                                 sizeof(g_devices[i].name),
                                 g_devices[i].name, 0) < 0)
            strcpy(g_devices[i].name, "[unknown]");
    }
    LOG_D("done");
    g_status = SPINO_BT_LOADED;
    return NULL;
}

void bluetooth_refresh_list(void)
{
    g_status = SPINO_BT_LOADING;
    pthread_create(&g_thread, NULL, thread_func, NULL);
}

int bluetooth_status(char *msg)
{
    static const char *STATUS_MSG[] = {
        [SPINO_BT_IDLE] = "Idle",
        [SPINO_BT_LOADING] = "Loading...",
        [SPINO_BT_NOT_AVAILABLE] = "No bluetooth_support",
        [SPINO_BT_ERROR] = "Error",
        [SPINO_BT_LOADED] = "Got list",
        [SPINO_BT_CONNECTED] = "Connected",
    };
    if (msg) sprintf(msg, "%s", STATUS_MSG[g_status]);
    return g_status;
}

void bluetooth_remember(const char *addr, const char *name)
{
    storage_set("bluetooth_last_addr", addr);
    storage_set("bluetooth_last_name", name);
}

void bluetooth_get_devices(void (*f)(const char *addr, const char *name))
{
    int i;
    if (g_status == SPINO_BT_IDLE && g_stored.addr) {
        f(g_stored.addr, g_stored.name);
    }
    for (i = 0; i < ARRAY_SIZE(g_devices); i++) {
        if (!g_devices[i].addr[0] || !g_devices[i].name[0]) return;
        f(g_devices[i].addr, g_devices[i].name);
    }
}

void bluetooth_connect(const char *dest)
{
    struct sockaddr_rc addr = { 0 };
    int status, flags;

    g_socket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t)1;
    str2ba(dest, &addr.rc_bdaddr);
    status = connect(g_socket, (struct sockaddr*)&addr, sizeof(addr));
    if (status == -1) {
        int errsv = errno;
        (void)errsv;
        perror("Error");
        g_status = SPINO_BT_ERROR;
        return;
    }
    flags = fcntl(g_socket, F_GETFL, 0);
    fcntl(g_socket, F_SETFL, flags | O_NONBLOCK);
    g_status = SPINO_BT_CONNECTED;
    return;
}

int bluetooth_send(const void *data, int size)
{
    int ret, err;
    int i;
    FILE *f;
    f=fopen("#comprimido.txt","a");
    ret = write(g_socket, data, size);
    for(i=0;i<size;i++){
	fprintf(f," %d \n",(int) (((unsigned char *)(data))[i]));
	

	}
    err = errno;
    if (ret == -1 && (err == EAGAIN || err == EWOULDBLOCK)) ret = 0;
    assert(ret >= 0);
    fclose(f);
    return ret;
}

int bluetooth_receive(void)
{
    int ret, err;
    uint8_t v;
    FILE *f;
    ret = read(g_socket, &v, 1);
    err = errno;
    f=fopen("#stream.txt","a");
	fprintf(f," %d \n",ret);
	fclose(f);
    if (ret == -1 && (err == EAGAIN || err == EWOULDBLOCK)) ret = 0;
    assert(ret >= 0);
    return ret;
}

#elif DEFINED(BLUETOOTH_OSX)

// On OSX, the core of the code is in the swift file.  We access the functions
// through callbacks.
static int g_status = SPINO_BT_IDLE;
static int g_recv = 0;

static int (*g_connect_callback)(void) = NULL;
static int (*g_send_callback)(const void *data, int size) = NULL;

void bluetooth_set_connect_callback(int (*f)(void))
{
    g_connect_callback = f;
}

void bluetooth_set_send_callback(int (*f)(const void *data, int size))
{
    g_send_callback = f;
}

void bluetooth_init(void) {}

void bluetooth_refresh_list(void)
{
    if (g_connect_callback() != 0)
        g_status = SPINO_BT_ERROR;
}

void bluetooth_signal(int type, int data)
{
    g_status = SPINO_BT_CONNECTED;
    if (type == 1) g_recv = 1;
}

void bluetooth_remember(const char *addr, const char *name) {}

int bluetooth_status(char *msg) {

    static const char *STATUS_MSG[] = {
        [SPINO_BT_IDLE] = "Idle",
        [SPINO_BT_LOADING] = "Loading...",
        [SPINO_BT_NOT_AVAILABLE] = "No bluetooth_support",
        [SPINO_BT_ERROR] = "Error",
        [SPINO_BT_LOADED] = "Got list",
        [SPINO_BT_CONNECTED] = "Connected",
    };
    if (msg) sprintf(msg, "%s", STATUS_MSG[g_status]);
    return g_status;
}

void bluetooth_get_devices(void (*f)(const char *addr, const char *name)) {}
void bluetooth_connect(const char *addr) {}

int bluetooth_send(const void *data, int size)
{
    return g_send_callback(data, size);
}

int bluetooth_receive(void)
{
    int ret = g_recv;
    if (ret) g_recv = 0;
    return ret;
}

#else

void bluetooth_init(void) {}
void bluetooth_refresh_list(void) {}
void bluetooth_remember(const char *addr, const char *name) {}
int bluetooth_status(char *msg) {
    if (msg) sprintf(msg, "No bluetooth support");
    return SPINO_BT_NOT_AVAILABLE;
}
void bluetooth_get_devices(void (*f)(const char *addr, const char *name)) {}
void bluetooth_connect(const char *addr) {}
int bluetooth_send(const void *data, int size) {return -1;}
int bluetooth_receive(void) {return -1;}

#endif
