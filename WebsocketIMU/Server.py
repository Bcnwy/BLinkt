import json
from websocket_server import WebsocketServer

<<<<<<< HEAD

=======
>>>>>>> origin/test
def new_client(client, server):
    print("Hey all, a new client has joined us")
    # start of EPOCH time


def message_received(client, server, message):
    if len(message) > 200:
        message = message[:200]+'..'
    data = json.loads(message)


# Called for every client disconnecting
def client_left(client, server):
    print("Client(%d) disconnected" % client['id'])


if __name__ == '__main__':
    server = WebsocketServer(81, host='0.0.0.0')
    server.set_fn_new_client(new_client)
    server.set_fn_client_left(client_left)
    server.set_fn_message_received(message_received)
    server.run_forever()
