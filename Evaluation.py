import sys
from skimage.metrics import peak_signal_noise_ratio as psnr
from skimage.metrics import structural_similarity as ssim
import cv2

def evaluate_images(original_image_path, compressed_image_path):
    # Read the original image and the compressed image in grayscale
    original = cv2.imread(original_image_path, cv2.IMREAD_GRAYSCALE)
    compressed = cv2.imread(compressed_image_path, cv2.IMREAD_GRAYSCALE)

    # Check if the images are loaded correctly
    if original is None:
        print(f"Error: Unable to load original image from {original_image_path}")
        return
    if compressed is None:
        print(f"Error: Unable to load compressed image from {compressed_image_path}")
        return

    # Calculate PSNR
    psnr_value = psnr(original, compressed)
    print(f"PSNR: {psnr_value} dB")

    # Calculate SSIM
    ssim_value, _ = ssim(original, compressed, full=True)
    print(f"SSIM: {ssim_value}")

if __name__ == "__main__":
    # Check if the correct number of arguments are provided
    if len(sys.argv) != 3:
        print("Usage: python Evaluation.py <original_image_path> <compressed_image_path>")
    else:
        # Extract image file paths from the command line arguments
        original_image_path = sys.argv[1]
        compressed_image_path = sys.argv[2]

        # Evaluate the images
        evaluate_images(original_image_path, compressed_image_path)
