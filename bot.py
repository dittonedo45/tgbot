import telebot
import os
import qr
import io
import typing
import threading

class TB(telebot.TeleBot):
    def reply_to(self, *args, **kwargs):
        print(args)
        telebot.TeleBot.reply_to(self, *args, **kwargs)
        pass

bot = TB (TOKEN) 

def persons():
    class BigBlob(typing.List):
        def append(s, *args, **kwargs):
            if (len (s)>=1000):
                s.pop ()
                pass
            return list.append(s, *args, **kwargs)
        pass
    pass
    my_bl=BigBlob ()
    class _Tracker(typing.Set):
        def __init__(self):
            self.count=0
        def add(self, p):
            my_bl.append (p.chat.id)
            self.count += 1
            set.add (self, p.chat.id)
            pass
        def __str__(s):
            return "<all <<%s>> <I hooked: %s>"%(" |\n".join (map(str, my_bl)),
                    ", ".join(map(str, s)),)
    return _Tracker()

my_clients = persons()

@bot.message_handler(commands=["hello"])
def say_hi_bob(msg):
    bot.reply_to(msg, "Hi !!!!, its me Eddington Chamunorwa's bot, Type any message, I will make you its QR encode.")

@bot.message_handler(func=lambda x: True)
def say_hi_calculator(msg):
    bot.reply_to(msg, qr._calc (msg.text))

@bot.message_handler(commands=["qr"])
def say(msg):
    b=io.BytesIO()
    qr._tostr (msg.text, b)
    bot.reply_to(msg, "Am transferring a QR code of: \"%s\""%(msg.text,))
    bot.send_photo(msg.chat.id, io.BytesIO(b.getvalue()), parse_mode="png")
    my_clients.add (msg)
    print(my_clients)
bot.infinity_polling()
