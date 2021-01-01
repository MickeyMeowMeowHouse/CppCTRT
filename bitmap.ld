ENTRY(dummy)

SECTIONS
{
	. = 0;
	.BitmapFile BLOCK(2) : SUBALIGN(2)
	{
		*(.BMFH);
		*(.BMIF);
		*(.Pixels);
		EndOfBitmap = .;
	}

	/DISCARD/ :
	{
		*(.text .text*)
	}
}

