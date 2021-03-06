"""LCM type definitions
This file automatically generated by lcm.
DO NOT MODIFY BY HAND!!!!
"""

try:
    import cStringIO.StringIO as BytesIO
except ImportError:
    from io import BytesIO
import struct

class hotspot_t(object):
    __slots__ = ["hotspot_id", "num_pixels", "center", "bounding", "max_temp", "avg_temp"]

    def __init__(self):
        self.hotspot_id = 0
        self.num_pixels = 0
        self.center = [ 0 for dim0 in range(2) ]
        self.bounding = [ 0 for dim0 in range(4) ]
        self.max_temp = 0.0
        self.avg_temp = 0.0

    def encode(self):
        buf = BytesIO()
        buf.write(hotspot_t._get_packed_fingerprint())
        self._encode_one(buf)
        return buf.getvalue()

    def _encode_one(self, buf):
        buf.write(struct.pack(">ii", self.hotspot_id, self.num_pixels))
        buf.write(struct.pack('>2i', *self.center[:2]))
        buf.write(struct.pack('>4i', *self.bounding[:4]))
        buf.write(struct.pack(">ff", self.max_temp, self.avg_temp))

    def decode(data):
        if hasattr(data, 'read'):
            buf = data
        else:
            buf = BytesIO(data)
        if buf.read(8) != hotspot_t._get_packed_fingerprint():
            raise ValueError("Decode error")
        return hotspot_t._decode_one(buf)
    decode = staticmethod(decode)

    def _decode_one(buf):
        self = hotspot_t()
        self.hotspot_id, self.num_pixels = struct.unpack(">ii", buf.read(8))
        self.center = struct.unpack('>2i', buf.read(8))
        self.bounding = struct.unpack('>4i', buf.read(16))
        self.max_temp, self.avg_temp = struct.unpack(">ff", buf.read(8))
        return self
    _decode_one = staticmethod(_decode_one)

    _hash = None
    def _get_hash_recursive(parents):
        if hotspot_t in parents: return 0
        tmphash = (0xd72ef5faf355c004) & 0xffffffffffffffff
        tmphash  = (((tmphash<<1)&0xffffffffffffffff)  + (tmphash>>63)) & 0xffffffffffffffff
        return tmphash
    _get_hash_recursive = staticmethod(_get_hash_recursive)
    _packed_fingerprint = None

    def _get_packed_fingerprint():
        if hotspot_t._packed_fingerprint is None:
            hotspot_t._packed_fingerprint = struct.pack(">Q", hotspot_t._get_hash_recursive([]))
        return hotspot_t._packed_fingerprint
    _get_packed_fingerprint = staticmethod(_get_packed_fingerprint)

