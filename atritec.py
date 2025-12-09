
# Source: https://foxglove.dev/blog/working-with-scenes-and-pointclouds
import os

import numpy as np

import foxglove
from foxglove.channels import (
    PointCloudChannel,
)
from foxglove.schemas import (
    Timestamp,
    PointCloud,
    PackedElementField,
    PackedElementFieldNumericType
)


f32 = PackedElementFieldNumericType.Float32
u32 = PackedElementFieldNumericType.Uint32
u16 = PackedElementFieldNumericType.Uint16

def main(): # convert my binary format to mcap
    fields = [
        PackedElementField(name="scan_number", offset=0, type=u32),
        PackedElementField(name="x", offset=4, type=f32),
        PackedElementField(name="y", offset=8, type=f32),
        PackedElementField(name="z", offset=12, type=f32),
        PackedElementField(name="intensity", offset=16, type=u16)
    ]
    struct_point = np.dtype(
        [
            ("scan_number", np.uint32),
            ("x", np.float32),
            ("y", np.float32),
            ("z", np.float32),
            ("intensity", np.uint16)
        ],
        align = False # Do not add the padding that a C compiler adds. Consequently, we can directly use np.fromfile to read the data struct by struct instead of field by field.
    )
    channel = PointCloudChannel(topic="point_cloud")
    with foxglove.open_mcap("output.mcap", allow_overwrite=True):
        data = np.fromfile("output.bin", dtype=struct_point)
        pc = PointCloud(
                timestamp=Timestamp(sec=0, nsec=0),
                frame_id="base",
                point_stride=struct_point.itemsize,
                fields=fields,
                data=data.tobytes(),
        )
        channel.log(
                pc,
                log_time=0,
        )



if __name__ == "__main__":
    main()