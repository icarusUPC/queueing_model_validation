"""LCM type definitions
This file automatically generated by lcm.
DO NOT MODIFY BY HAND!!!!
"""

try:
    import cStringIO.StringIO as BytesIO
except ImportError:
    from io import BytesIO
import struct

import messages.hotspot_t

class hotspot_ret_t(object):
    __slots__ = ["command_id", "result", "hotspot_count", "hotspot_list"]

    def __init__(self):
        self.command_id = 0
        self.result = 0
        self.hotspot_count = 0
        self.hotspot_list = []

    def encode(self):
        buf = BytesIO()
        buf.write(hotspot_ret_t._get_packed_fingerprint())
        self._encode_one(buf)
        return buf.getvalue()

    def _encode_one(self, buf):
        buf.write(struct.pack(">qBi", self.command_id, self.result, self.hotspot_count))
        for i0 in range(self.hotspot_count):
            assert self.hotspot_list[i0]._get_packed_fingerprint() == messages.hotspot_t._get_packed_fingerprint()
            self.hotspot_list[i0]._encode_one(buf)

    def decode(data):
        if hasattr(data, 'read'):
            buf = data
        else:
            buf = BytesIO(data)
        if buf.read(8) != hotspot_ret_t._get_packed_fingerprint():
            raise ValueError("Decode error")
        return hotspot_ret_t._decode_one(buf)
    decode = staticmethod(decode)

    def _decode_one(buf):
        self = hotspot_ret_t()
        self.command_id, self.result, self.hotspot_count = struct.unpack(">qBi", buf.read(13))
        self.hotspot_list = []
        for i0 in range(self.hotspot_count):
            self.hotspot_list.append(messages.hotspot_t._decode_one(buf))
        return self
    _decode_one = staticmethod(_decode_one)

    _hash = None
    def _get_hash_recursive(parents):
        if hotspot_ret_t in parents: return 0
        newparents = parents + [hotspot_ret_t]
        tmphash = (0x8eba74c160db7a27+ messages.hotspot_t._get_hash_recursive(newparents)) & 0xffffffffffffffff
        tmphash  = (((tmphash<<1)&0xffffffffffffffff)  + (tmphash>>63)) & 0xffffffffffffffff
        return tmphash
    _get_hash_recursive = staticmethod(_get_hash_recursive)
    _packed_fingerprint = None

    def _get_packed_fingerprint():
        if hotspot_ret_t._packed_fingerprint is None:
            hotspot_ret_t._packed_fingerprint = struct.pack(">Q", hotspot_ret_t._get_hash_recursive([]))
        return hotspot_ret_t._packed_fingerprint
    _get_packed_fingerprint = staticmethod(_get_packed_fingerprint)

