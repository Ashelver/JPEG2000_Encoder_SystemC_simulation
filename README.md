Evronment requirements:

`libpng` and `systemC`


Command for encoder:
```bash
./jpeg2000_encoder.x <image_path> <output_path> <compression_option>
```

Command for decoder:
```bash
./jpeg2000_decoder <jp2_file_path> <output_path>
```

You can use the script I provided:
```bash
bash script
```


Evaluation:
Fist install python library:
```bash
pip install scikit-image
pip install opencv-python
```
```bash
python Evaluation.py <path_to_original_image> <path_to_compressed_image>
```
