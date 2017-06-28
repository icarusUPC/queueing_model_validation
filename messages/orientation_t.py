"""LCM type definitions
This file automatically generated by lcm.
DO NOT MODIFY BY HAND!!!!
"""

try:
    import cStringIO.StringIO as BytesIO
except ImportError:
    from io import BytesIO
import struct

class orientation_t(object):
    __slots__ = ["yaw", "pitch", "roll"]

    def __init__(self):
        self.yaw = 0.0
        self.pitch = 0.0
        self.roll = 0.0

    def encode(self):
        buf = BytesIO()
        buf.write(orientation_t._get_packed_fingerprint())
        self._encode_one(buf)
        return buf.getvalue()

    def _encode_one(self, buf):
        buf.write(struct.pack(">ddd", self.yaw, self.pitch, self.roll))

    def decode(data):
        if hasattr(data, 'read'):
            buf = data
        else:
            buf = BytesIO(data)
        if buf.read(8) != orientation_t._get_packed_fingerprint():
            raise ValueError("Decode error")
        return orientation_t._decode_one(buf)
    decode = staticmethod(decode)

    def _decode_one(buf):
        self = orientation_t()
        self.yaw, self.pitch, self.roll = struct.unpack(">ddd", buf.read(24))
        return self
    _decode_one = staticmethod(_decode_one)

    _hash = None
    def _get_hash_recursive(parents):
        if orientation_t in parents: return 0
        tmphash = (0x30c12aceba8908d8) & 0xffffffffffffffff
        tmphash  = (((tmphash<<1)&0xffffffffffffffff)  + (tmphash>>63)) & 0xffffffffffffffff
        return tmphash
    _get_hash_recursive = staticmethod(_get_hash_recursive)
    _packed_fingerprint = None

    def _get_packed_fingerprint():
        if orientation_t._packed_fingerprint is None:
            orientation_t._packed_fingerprint = struct.pack(">Q", orientation_t._get_hash_recursive([]))
        return orientation_t._packed_fingerprint
    _get_packed_fingerprint = staticmethod(_get_packed_fingerprint)

