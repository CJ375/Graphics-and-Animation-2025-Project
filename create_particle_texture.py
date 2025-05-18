"""
Enhanced script to create a circular particle texture with soft gradient edges.
Requires PIL (Python Imaging Library) which is part of Pillow.
"""

from PIL import Image, ImageDraw, ImageFilter
import os
import numpy as np

def create_particle_texture(size=128, output_path="res/textures/particle/default_particle.png"):
    # Create a transparent image
    img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    # Draw a white circle with soft edges
    center = size // 2
    radius = size // 3
    
    # Draw the main circle (white) with a larger radius for the gradient
    draw.ellipse((center-radius, center-radius, center+radius, center+radius), 
                 fill=(255, 255, 255, 255))
    
    # Apply a gaussian blur to create soft edges
    img = img.filter(ImageFilter.GaussianBlur(radius=radius/4))
    
    # Enhance the center brightness to make it more visible
    data = np.array(img)
    # Create a radial gradient mask
    y, x = np.ogrid[:size, :size]
    dist_from_center = np.sqrt((x - center)**2 + (y - center)**2)
    mask = dist_from_center <= radius/2
    
    # Brighten the center
    data[mask, 3] = 255  # Set alpha to full
    
    # Convert back to an image
    img = Image.fromarray(data)
    
    # Save the image
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    img.save(output_path)
    print(f"Created enhanced particle texture at {output_path}")

if __name__ == "__main__":
    create_particle_texture() 