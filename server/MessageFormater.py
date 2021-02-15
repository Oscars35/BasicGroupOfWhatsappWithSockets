from enum import Enum

class ColorSelection(Enum):
    SUCCESS = "\033[0;32m"
    WARNING = "\033[0;31m"
    FINAL_COLOR = "\033[0m"

class ColorList(Enum):
    WHITE = "\033[0m"
    BLUE = "\033[34m"
    ORANGE = "\033[0;33m"
    YELLOW = "\033[0;33m"
    PURPLE = "\033[0;35m"
    CIAN = "\033[0;36m"
    LIGHT_RED = "\033[1;31m"

class Formater:

    @staticmethod
    def message_send(color, message, address):
        message = color.value + address + "--> " + message + ColorSelection.FINAL_COLOR.value
        return message

    @staticmethod
    def connection_stablished_client():
        return ColorSelection.SUCCESS.value + "Connected to the server! " + ColorSelection.FINAL_COLOR.value 

    @staticmethod
    def connection_stablished_server(nickName):
        return ColorSelection.SUCCESS.value + f"{nickName} connected to the chat!" + ColorSelection.FINAL_COLOR.value

    @staticmethod
    def left_chat(nickName):
        return Formater.message(ColorSelection.WARNING, f"{nickName} has left the chat")

    @staticmethod
    def joined(nickName):
        return Formater.message(ColorSelection.SUCCESS, f"{nickName} joined!")


    @staticmethod
    def message(color, message):
        return color.value + message + ColorSelection.FINAL_COLOR.value



