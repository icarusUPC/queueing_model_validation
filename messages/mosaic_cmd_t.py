"""LCM type definitions
This file automatically generated by lcm.
DO NOT MODIFY BY HAND!!!!
"""

try:
    import cStringIO.StringIO as BytesIO
except ImportError:
    from io import BytesIO
import struct

import messages.geolocation_t

import messages.orientation_t

class mosaic_cmd_t(object):
    __slots__ = ["command_id", "photo_file", "photo_location", "photo_attitude"]

    def __init__(self):
        self.command_id = 0
        self.photo_file = [ "" for dim0 in range(4) ]
        self.photo_location = [ messages.geolocation_t() for dim0 in range(4) ]
        self.photo_attitude = [ messages.orientation_t() for dim0 in range(4) ]

    def encode(self):
        buf = BytesIO()
        buf.write(mosaic_cmd_t._get_packed_fingerprint())
        self._encode_one(buf)
        return buf.getvalue()

    def _encode_one(self, buf):
        buf.write(struct.pack(">q", self.command_id))
        for i0 in range(4):
            __photo_file_encoded = self.photo_file[i0].encode('utf-8')
            buf.write(struct.pack('>I', len(__photo_file_encoded)+1))
            buf.write(__photo_file_encoded)
            buf.write(b"\0")
        for i0 in range(4):
            assert self.photo_location[i0]._get_packed_fingerprint() == messages.geolocation_t._get_packed_fingerprint()
            self.photo_location[i0]._encode_one(buf)
        for i0 in range(4):
            assert self.photo_attitude[i0]._get_packed_fingerprint() == messages.orientation_t._get_packed_fingerprint()
            self.photo_attitude[i0]._encode_one(buf)

    def decode(data):
        if hasattr(data, 'read'):
            buf = data
        else:
            buf = BytesIO(data)
        if buf.read(8) != mosaic_cmd_t._get_packed_fingerprint():
            raise ValueError("Decode error")
        return mosaic_cmd_t._decode_one(buf)
    decode = staticmethod(decode)

    def _decode_one(buf):
        self = mosaic_cmd_t()
        self.command_id = struct.unpack(">q", buf.read(8))[0]
        self.photo_file = []
        for i0 in range(4):
            __photo_file_len = struct.unpack('>I', buf.read(4))[0]
            self.photo_file.append(buf.read(__photo_file_len)[:-1].decode('utf-8', 'replace'))
        self.photo_location = []
        for i0 in range(4):
            self.photo_location.append(messages.geolocation_t._decode_one(buf))
        self.photo_attitude = []
        for i0 in range(4):
            self.photo_attitude.append(messages.orientation_t._decode_one(buf))
        return self
    _decode_one = staticmethod(_decode_one)

    _hash = None
    def _get_hash_recursive(parents):
        if mosaic_cmd_t in parents: return 0
        newparents = parents + [mosaic_cmd_t]
        tmphash = (0x2db462e1124c7063+ messages.geolocation_t._get_hash_recursive(newparents)+ messages.orientation_t._get_hash_recursive(newparents)) & 0xffffffffffffffff
        tmphash  = (((tmphash<<1)&0xffffffffffffffff)  + (tmphash>>63)) & 0xffffffffffffffff
        return tmphash
    _get_hash_recursive = staticmethod(_get_hash_recursive)
    _packed_fingerprint = None

    def _get_packed_fingerprint():
        if mosaic_cmd_t._packed_fingerprint is None:
            mosaic_cmd_t._packed_fingerprint = struct.pack(">Q", mosaic_cmd_t._get_hash_recursive([]))
        return mosaic_cmd_t._packed_fingerprint
    _get_packed_fingerprint = staticmethod(_get_packed_fingerprint)

