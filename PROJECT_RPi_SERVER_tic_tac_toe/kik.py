from distutils.log import error
from operator import contains
from paho.mqtt import client as mqtt_client
from enum import Enum

class STATUS(Enum):
    WAITINGFORPLAYERS = 1
    GAME = 2

broker = "192.168.43.36" # $hostname -I
port = 1883
client_id = f'RPi_python_Client'

##
# gameserver
# password0
# 


class Board:
    def __init__(self):
        self.board =    [-1, -1, -1, 
                        -1, -1, -1,
                        -1, -1, -1]

    def putMarker(self, pos, player):
        if not (player == 1 or player == 2):
            return "Wrong player identificator"
        if self.board[pos] == -1:
            self.board[pos] = player
            print("move recorded")
            return 0
        else: 
            return "position taken"
    
    def checkIfGameFinished(self):
        #row
        for i in range(3):
            if self.board[i*3] != -1 and self.board[i*3] == self.board[i*3+1] == self.board[i*3+2]:
                return self.board[i*3]
        #col
        for i in range(3):
            if self.board[i] != -1 and self.board[i] == self.board[i+3] == self.board[i+6]:
                return self.board[i]
        #diagonal
        if self.board[4] != -1 and ((self.board[0] == self.board[4] == self.board[8]) or (self.board[2] == self.board[4] == self.board[6])):
            return self.board[4]
        return False
    
    def clearBoard(self):
        self.board =    [-1, -1, -1, 
                        -1, -1, -1,
                        -1, -1, -1]
        
    def getBoardString(self):
        board = ""
        for i in range(9):
            if self.board[i] == -1:
                board = board + str(3)
            else:
                board = board + str(self.board[i])
        return board
            

class Game:
    def __init__(self):
        self.board = Board()
        self.turn = 1
        self.playersCount = 1

    def putMarker(self, pos, playerId):
        if self.turn != playerId:
            return 'Not your turn'
        if playerId < 1 and playerId > 2:
            return 'Wrong player Id'
        retVal = self.board.putMarker(pos, playerId)
        if retVal == 'position taken':
            return retVal
        winner = self.board.checkIfGameFinished()
        if winner:
            self.board.clearBoard()
            print('Player' + str(winner) + ' won!')
            return winner
        else:
            self.turn = self.turn%2+1
            return retVal

def connect_mqtt() -> mqtt_client:
    print("connect mqtt:")
    def on_connect(client, userdata, flags, rc):
        print("on connect")
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    client = mqtt_client.Client(client_id)
    client.username_pw_set("gameserver", "password0")
    client.on_connect = on_connect
    client.connect(broker, port)
    return client

game = Game()


def subscribe(client: mqtt_client):
    def on_message(client, userdata, msg):
        msgString = msg.payload.decode()
        print(f"Received `{msg.payload.decode()}` from `{msg.topic}` topic")
        
        # KLIENT ZAJMUJE ID -> PUBLIKUJ NASTĘPNE ID
        if msg.topic == 'gameclients' and msgString == 'subscribed':
            if game.playersCount == 1:
                game.playersCount = 2
                client.publish("nextclientid", game.playersCount, retain=True)
                print("first client")
                print(game.board.getBoardString())
                client.publish("board", game.board.getBoardString(), retain=True)
            elif game.playersCount == 2:
                client.publish("turn", 1, retain=True)
                print("second client")
                print("start game")
                print(game.board.getBoardString())
                client.publish("board", game.board.getBoardString(), retain=True)
        
        if msg.topic == 'client1':
            move = int(msgString)
            print("MOVE1 = " + str(move))
            response = game.putMarker(move, 1)
            print("RESPONSE = " + str(response))
            if response == 0:
                client.publish("turn", 2, retain=True)
            elif response == 1 or response == 2:
                client.publish("won", response)
            else:
                client.publish("turn", 1, retain=True)
            print(game.board.getBoardString())
            client.publish("board", game.board.getBoardString(), retain=True)
            
        if msg.topic == 'client2':
            move = int(msgString)
            print("MOVE2 = " + str(move))
            response = game.putMarker(move, 2)
            print("RESPONSE = " + str(response))
            if response == 0:
                client.publish("turn", 1, retain=True)
            elif response == 1 or response == 2:
                client.publish("won", response)
            else:
                client.publish("turn", 2, retain=True)
            print(game.board.getBoardString())
            client.publish("board", game.board.getBoardString(), retain=True)
            


    client.subscribe('gameclients')
    client.subscribe('client1')
    client.subscribe('client2')
    
    client.on_message = on_message

def run():
    print("python script run...")
    client = connect_mqtt()
    print("mqtt connected...")
    subscribe(client)
    # PUBLIKUJ PIERWSZE ID DO ZAJĘCIA
    client.publish("nextclientid", game.playersCount, retain=True)
    # PUBLIKUJ RUCH 0 -> OCZEKIWANIE NA GRACZY
    client.publish("turn", 0, retain=True)
    
    client.loop_forever()


if __name__ == '__main__':
    run()

    
