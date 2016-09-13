import numpy as np
import sys
from PIL import Image
import argparse
from intelhex import IntelHex
import os

def convert_grayscale_array(image):
	image=image.convert('L')
	return np.asarray(image.getdata(), dtype=np.float64)

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description='Encode images into a hex file.')
	parser.add_argument('--format', default='bit', help='Encoded image format. Supported: bit, uint8, float')
	parser.add_argument('--start', default=0, type=int, help='Where to start storing images')
	parser.add_argument('--output', default='out', help='Name of output file')
	parser.add_argument('file', nargs='+', help='list of files to encode')

	args=parser.parse_args()
	ih = IntelHex()
	position=args.start
	target = open(args.output+'.h', 'w')
	target.write('/*Auto generated! Don\'t touch!*/\n')
	target.write('#ifndef GPA_IMAGE_TABLE\n')
	target.write('#define GPA_IMAGE_TABLE\n')
	target.write('#include <avr/eeprom.h>\n')


	for image_file in args.file:
		print("Saving %s @ %i"%(image_file, position))
		image = Image.open(image_file,'r')

		#if image.size[0]%8 != 0:
		#	print('Image width must be a multiple of 8!')
		array=convert_grayscale_array(image)
		if args.format=='bit':
			array=np.packbits(array>np.average(array))
		elif args.format=='uint8':
			array=array.astype(np.uint8)
		elif args.format=='float':
			pass
		else:
			print("Invalid format %s! Assuming float...")
			print(array)
		name=os.path.basename(image_file).split('.')[0].upper()
		target.write('/*Image name: %s*/\n'%(os.path.basename(image_file).split('.')[0]))
		target.write('\t#define %s_OFFSET %i\n'%(name, position))
		target.write('\t#define %s_WIDTH %i\n'%(name, image.size[0]))
		target.write('\t#define %s_HEIGHT %i\n'%(name, image.size[1]))
		target.write('\t#define %s_SIZE %i\n'%(name, array.size))
		if args.format=='bit':
			target.write('\t#define %s_GET_PIXEL(x,y) (eeprom_read_byte((uint8_t*)(%s_OFFSET+y*%s_WIDTH+x)) & (1 << x) != 0)\n'%(name,name,name))
		elif args.format=='uint8':
			target.write('\t#define %s_GET_PIXEL(x,y) eeprom_read_byte((uint8_t*)(%s_OFFSET+y*%s_WIDTH+x))\n'%(name,name,name))
		elif args.format=='float':
			target.write('\t#define %s_GET_PIXEL(x,y) eeprom_read_float((uint8_t*)(%s_OFFSET+y*%s_WIDTH*4+x*4))\n'%(name,name,name))
		
		target.write('\n')
		ih.puts(position, array.tostring())
		position=position+array.size
	target.write('#endif\n')
	target.close()
	ih.tofile(args.output+'.hex', format='hex')
	print("Wrote %i bytes!"%position)







	