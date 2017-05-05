import json
from websocket_server import WebsocketServer
from blinkt import set_brightness, set_pixel, show, clear
import time
import colorsys


def new_client(client, server):
    print("Hey all, a new client has joined us")
    # start of EPOCH time


def message_received(client, server, message):
    if len(message) > 200:
        message = message[:200]+'..'
    data = json.loads(message)
    pixel = int(abs((data['y']/180) * 8 + 4))
    print"Pixel: {}".format(pixel)
    print int(data['x']), data['x']/360
    r, g, b = [int(c * 255) for c in colorsys.hsv_to_rgb((data['x']/360), 1.0, 1.0)]
    clear()
    set_pixel(pixel, r, g, b)
    show()


# Called for every client disconnecting
def client_left(client, server):
    print("Client(%d) disconnected" % client['id'])


if __name__ == '__main__':
    spacing = 360.0 / 16.0
    hue = 0
    hue = int(time.time() * 100) % 360
    for x in range(8):
        offset = x * spacing
        h = ((hue + offset) % 360) / 360.0
        r, g, b = [int(c * 255) for c in colorsys.hsv_to_rgb(h, 1.0, 1.0)]
        set_pixel(x, r, g, b)
    show()
    time.sleep(1)
    clear()

    server = WebsocketServer(81, host='0.0.0.0')
    server.set_fn_new_client(new_client)
    server.set_fn_client_left(client_left)
    server.set_fn_message_received(message_received)
    server.run_forever()
