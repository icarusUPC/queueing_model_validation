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

class georef_cmd_t(object):
    __slots__ = ["command_id", "pixel", "location", "attitude"]

    def __init__(self):
        self.command_id = 0
        self.pixel = [ 0 for dim0 in range(2) ]
        self.location = messages.geolocation_t()
        self.attitude = messages.orientation_t()

    def encode(self):
        buf = BytesIO()
        buf.write(georef_cmd_t._get_packed_fingerprint())
        self._encode_one(buf)
        return buf.getvalue()

    def _encode_one(self, buf):
        buf.write(struct.pack(">q", self.command_id))
        buf.write(struct.pack('>2i', *self.pixel[:2]))
        assert self.location._get_packed_fingerprint() == messages.geolocation_t._get_packed_fingerprint()
        self.location._encode_one(buf)
        assert self.attitude._get_packed_fingerprint() == messages.orientation_t._get_packed_fingerprint()
        self.attitude._encode_one(buf)

    def decode(data):
        if hasattr(data, 'read'):
            buf = data
        else:
            buf = BytesIO(data)
        if buf.read(8) != georef_cmd_t._get_packed_fingerprint():
            raise ValueError("Decode error")
        return georef_cmd_t._decode_one(buf)
    decode = staticmethod(decode)

    def _decode_one(buf):
        self = georef_cmd_t()
        self.command_id = struct.unpack(">q", buf.read(8))[0]
        self.pixel = struct.unpack('>2i', buf.read(8))
        self.location = messages.geolocation_t._decode_one(buf)
        self.attitude = messages.orientation_t._decode_one(buf)
        return self
    _decode_one = staticmethod(_decode_one)

    _hash = None
    def _get_hash_recursive(parents):
        if georef_cmd_t in parents: return 0
        newparents = parents + [georef_cmd_t]
        tmphash = (0x6af7f044903256ca+ messages.geolocation_t._get_hash_recursive(newparents)+ messages.orientation_t._get_hash_recursive(newparents)) & 0xffffffffffffffff
        tmphash  = (((tmphash<<1)&0xffffffffffffffff)  + (tmphash>>63)) & 0xffffffffffffffff
        return tmphash
    _get_hash_recursive = staticmethod(_get_hash_recursive)
    _packed_fingerprint = None

    def _get_packed_fingerprint():
        if georef_cmd_t._packed_fingerprint is None:
            georef_cmd_t._packed_fingerprint = struct.pack(">Q", georef_cmd_t._get_hash_recursive([]))
        return georef_cmd_t._packed_fingerprint
    _get_packed_fingerprint = staticmethod(_get_packed_fingerprint)
