import struct

class Model:
    MAGIC = b"sha"
    VERSION = 1

    TYPE_INT_LIST = 0
    TYPE_STR_LIST = 1
    TYPE_INT = 2
    TYPE_DICT_INT_STR = 3
    TYPE_DICT_INT_INT = 4
    TYPE_DICT_STR_INT = 5

    def __init__(self, filename: str, datas: list) -> None:    
        self.filename = filename    
        self.datas = datas    

    
    def save(self) -> None:
        with open(self.filename, "wb") as file:
            file.write(self.MAGIC)
            file.write(struct.pack("B", self.VERSION))
            file.write(struct.pack("<I", len(self.datas)))

            for block in self.datas:

                # int list
                if isinstance(block, list) and all(
                    isinstance(x, int) for x in block
                ):
                    file.write(struct.pack("B", self.TYPE_INT_LIST))
                    file.write(struct.pack("<I", len(block)))

                    for num in block:
                        file.write(struct.pack("<i", num))

                # string list
                elif isinstance(block, list) and all(
                    isinstance(x, str) for x in block
                ):
                    file.write(struct.pack("B", self.TYPE_STR_LIST))
                    file.write(struct.pack("<I", len(block)))

                    for text in block:
                        data = text.encode("utf-8")
                        file.write(struct.pack("<I", len(data)))
                        file.write(data)

                # standalone int
                elif isinstance(block, int):
                    file.write(struct.pack("B", self.TYPE_INT))
                    file.write(struct.pack("<i", block))

                # dict[int, str]
                elif (
                    isinstance(block, dict)
                    and all(isinstance(k, int) for k in block)
                    and all(isinstance(v, str) for v in block.values())
                ):
                    file.write(struct.pack("B", self.TYPE_DICT_INT_STR))
                    file.write(struct.pack("<I", len(block)))

                    for k, v in block.items():
                        data = v.encode("utf-8")

                        file.write(struct.pack("<i", k))
                        file.write(struct.pack("<I", len(data)))
                        file.write(data)

                # dict[int, int]
                elif (
                    isinstance(block, dict)
                    and all(isinstance(k, int) for k in block)
                    and all(isinstance(v, int) for v in block.values())
                ):
                    file.write(struct.pack("B", self.TYPE_DICT_INT_INT))
                    file.write(struct.pack("<I", len(block)))

                    for k, v in block.items():
                        file.write(struct.pack("<i", k))
                        file.write(struct.pack("<i", v))
                elif (
                    isinstance(block, dict)
                    and all(isinstance(k, str) for k in block)
                    and all(isinstance(v, int) for v in block.values())
                ):
                    file.write(struct.pack("B", self.TYPE_DICT_STR_INT))
                    file.write(struct.pack("<I", len(block)))

                    for k, v in block.items():
                        key_data = k.encode("utf-8")

                        file.write(struct.pack("<I", len(key_data)))
                        file.write(key_data)
                        file.write(struct.pack("<i", v))
                        
                else:
                    raise TypeError(f"Unsupported type: {type(block)}")

    def load(self) -> list:
        with open(self.filename, "rb") as f:

            if f.read(len(self.MAGIC)) != self.MAGIC:
                raise ValueError("Invalid file format.")

            version = struct.unpack("B", f.read(1))[0]
            block_count = struct.unpack("<I", f.read(4))[0]

            result = []

            for _ in range(block_count):

                block_type = struct.unpack("B", f.read(1))[0]

                if block_type == self.TYPE_INT_LIST:

                    length = struct.unpack("<I", f.read(4))[0]
                    result.append(
                        [struct.unpack("<i", f.read(4))[0] for _ in range(length)]
                    )

                elif block_type == self.TYPE_STR_LIST:

                    length = struct.unpack("<I", f.read(4))[0]
                    items = []

                    for _ in range(length):
                        size = struct.unpack("<I", f.read(4))[0]
                        items.append(f.read(size).decode("utf-8"))

                    result.append(items)

                elif block_type == self.TYPE_INT:

                    result.append(struct.unpack("<i", f.read(4))[0])

                elif block_type == self.TYPE_DICT_INT_STR:

                    length = struct.unpack("<I", f.read(4))[0]
                    d = {}

                    for _ in range(length):
                        key = struct.unpack("<i", f.read(4))[0]

                        size = struct.unpack("<I", f.read(4))[0]
                        value = f.read(size).decode("utf-8")

                        d[key] = value

                    result.append(d)

                elif block_type == self.TYPE_DICT_INT_INT:

                    length = struct.unpack("<I", f.read(4))[0]
                    d = {}

                    for _ in range(length):
                        key = struct.unpack("<i", f.read(4))[0]
                        value = struct.unpack("<i", f.read(4))[0]

                        d[key] = value

                    result.append(d)
                elif block_type == self.TYPE_DICT_STR_INT:

                    length = struct.unpack("<I", f.read(4))[0]
                    d = {}

                    for _ in range(length):
                        size = struct.unpack("<I", f.read(4))[0]
                        key = f.read(size).decode("utf-8")

                        value = struct.unpack("<i", f.read(4))[0]

                        d[key] = value

                    result.append(d)

                else:
                    raise ValueError(f"Unknown block type: {block_type}")

        return result
