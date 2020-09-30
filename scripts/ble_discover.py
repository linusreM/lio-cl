import platform
import logging
import asyncio

from bleak import BleakClient
from bleak import _logger as logger
from bleak.uuids import uuid16_dict

RX_UUID = "49535343-1e4d-4bd9-ba61-23c647249616"
TX_UUID = "49535343-8841-43f4-a8d4-ecbe34729bb3"

global current_string
async def run(address, loop, debug=False):
    if debug:
        import sys

        # loop.set_debug(True)
        # l = logging.getLogger("asyncio")
        # l.setLevel(logging.DEBUG)
        # h = logging.StreamHandler(sys.stdout)
        # h.setLevel(logging.DEBUG)
        # l.addHandler(h)
    

    async with BleakClient(address, timeout=1.0, loop=loop) as client:

        queue = asyncio.Queue()
        

        def keypress_handler(sender, data):
            
            print('{}'.format(data.decode('ascii')), end='', flush=True)
            queue.put_nowait(data)

        x = await client.is_connected()
        logger.info("Connected: {0}".format(x))
        current_string = ""
        await client.start_notify(RX_UUID, keypress_handler)
        while(1):
            await asyncio.sleep(0.1, loop=loop)
            try:
                send_data = queue.get_nowait()
                await client.write_gatt_char(TX_UUID, send_data)
            except:
            	pass
        await client.stop_notify(RX_UUID)


if __name__ == "__main__":
    import os
    current_string = ""
    os.environ["PYTHONASYNCIODEBUG"] = str(1)
    address = (
        "04:91:62:A3:15:2B"
        if platform.system() != "Darwin"
        else "B9EA5233-37EF-4DD6-87A8-2A875E821C46"
    )
    loop = asyncio.get_event_loop()
    loop.run_until_complete(run(address, loop, True))