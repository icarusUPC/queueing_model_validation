"""LCM type definitions
This file automatically generated by lcm.
DO NOT MODIFY BY HAND!!!!
"""

try:
    import cStringIO.StringIO as BytesIO
except ImportError:
    from io import BytesIO
import struct

class jellyfish_cmd_t(object):
    __slots__ = ["command_id", "photo_file"]

    def __init__(self):
        self.command_id = 0
        self.photo_file = ""

    def encode(self):
        buf = BytesIO()
        buf.write(jellyfish_cmd_t._get_packed_fingerprint())
        self._encode_one(buf)
        return buf.getvalue()

    def _encode_one(self, buf):
        buf.write(struct.pack(">q", self.command_id))
        __photo_file_encoded = self.photo_file.encode('utf-8')
        buf.write(struct.pack('>I', len(__photo_file_encoded)+1))
        buf.write(__photo_file_encoded)
        buf.write(b"\0")

    def decode(data):
        if hasattr(data, 'read'):
            buf = data
        else:
            buf = BytesIO(data)
        if buf.read(8) != jellyfish_cmd_t._get_packed_fingerprint():
            raise ValueError("Decode error")
        return jellyfish_cmd_t._decode_one(buf)
    decode = staticmethod(decode)

    def _decode_one(buf):
        self = jellyfish_cmd_t()
        self.command_id = struct.unpack(">q", buf.read(8))[0]
        __photo_file_len = struct.unpack('>I', buf.read(4))[0]
        self.photo_file = buf.read(__photo_file_len)[:-1].decode('utf-8', 'replace')
        return self
    _decode_one = staticmethod(_decode_one)

    _hash = None
    def _get_hash_recursive(parents):
        if jellyfish_cmd_t in parents: return 0
        tmphash = (0xa0fe976675f33ac1) & 0xffffffffffffffff
        tmphash  = (((tmphash<<1)&0xffffffffffffffff)  + (tmphash>>63)) & 0xffffffffffffffff
        return tmphash
    _get_hash_recursive = staticmethod(_get_hash_recursive)
    _packed_fingerprint = None

    def _get_packed_fingerprint():
        if jellyfish_cmd_t._packed_fingerprint is None:
            jellyfish_cmd_t._packed_fingerprint = struct.pack(">Q", jellyfish_cmd_t._get_hash_recursive([]))
        return jellyfish_cmd_t._packed_fingerprint
    _get_packed_fingerprint = staticmethod(_get_packed_fingerprint)

