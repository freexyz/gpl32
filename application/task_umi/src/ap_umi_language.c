#include "ap_umi_language.h"


#define UNI_FONT_DB_LEN_MAX     90000
#define UNI_FONT_DB_HEADER_MAX  90000

#if SUPPORT_UNICODE_FILE_NAME == CUSTOM_ON
void ap_umi_uni_string_draw(GraphicBitmap *bitmap,INT8S *strings,GraphicCoordinates *coordinates,INT8U ascii_font_type);
INT32U ap_umi_lang_strings_width_get_for_no_newline(INT8S *strings,INT8U ascii_font_type);
INT32U ap_umi_lang_strings_height_get(INT8S *strings,INT8U ascii_font_type);

void ap_umi_uni_char_draw(GraphicBitmap *bitmap,unsigned short uni_code,GraphicCoordinates *coordinates,INT8U ascii_font_type);
INT32S ap_umi_lang_font_get(INT16U uni_code, t_FONT_TABLE_STRUCT *font_table);
INT32U ap_umi_lang_font_width_get(INT16U uni_code,INT8U ascii_font_type);
INT32U ap_umi_lang_font_height_get(INT16U uni_code,INT8U ascii_font_type);
INT32S ap_umi_lang_uni_tbl_search(INT16U uni_code);
void ap_umi_lang_font_data_adj(INT8U height,INT8U byte_per_line,INT8U* buf);
void ap_umi_lang_font_data_adj1(INT8U height,INT8U byte_per_line,INT8U *src_buf,INT8U *dest_buf);

st_uni_range_tbl *uni_range_tbl;
INT32U total_fonts;
INT16U fontdb_hd;

void ap_umi_language_init(void)
{
	INT32U len;
	INT32U header_len;
	INT8U  buf[16];
	INT32S i,j;
	st_uni_range_tbl tmp_uni_rabge_tbl;
	
	fontdb_hd = nv_open((INT8U *) "UNI_FONTDB.FT");
	if (fontdb_hd == 0xFFFF) {
		DBG_PRINT("Failed to open font db resource file\r\n");
		return;
	}	
	
	if (nv_read(fontdb_hd, (INT32U) buf, 8)) {
	    DBG_PRINT("Failed to read font data in ap_umi_language_init()\r\n");
		return ;
	}
	
	if (gp_memcmp((INT8S*) buf,(INT8S*)"fontdb",6) != 0 ) {
		DBG_PRINT("invalid font db\r\n");
		return;
	}
	
	total_fonts = (buf[6])|(buf[7]<<8);
	
	uni_range_tbl = (st_uni_range_tbl *) gp_malloc(sizeof(st_uni_range_tbl)*total_fonts);
	if (!uni_range_tbl || !total_fonts || total_fonts > 255) {
		DBG_PRINT("ap_umi_language_init fail\r\n");
		return;
	}
	
	gp_memset((INT8S*)uni_range_tbl,0,sizeof(st_uni_range_tbl)*total_fonts);
	
	/* create unicode code range table */
	for (i=0;i<total_fonts;i++) {
		if (nv_read(fontdb_hd, (INT32U) buf, 16)) {
	    	DBG_PRINT("ap_umi_language_init fail\r\n");
	    	gp_free(uni_range_tbl);
			return ;
		}
		
		uni_range_tbl[i].base_addr = nv_lseek(fontdb_hd,0,SEEK_CUR);
		 
		if (gp_memcmp((INT8S*)buf,(INT8S*)"ucs2",4) != 0 ) {
			DBG_PRINT("ap_umi_language_init fail\r\n");
	    	gp_free(uni_range_tbl);
			return ;		
		}
		uni_range_tbl[i].start_value = (buf[12])|(buf[13]<<8);
		uni_range_tbl[i].end_value = (buf[14])|(buf[15]<<8);	
		uni_range_tbl[i].font_byte_per_line = buf[7];
		uni_range_tbl[i].font_height = buf[6];
		len = (buf[8])|(buf[9]<<8)|(buf[10]<<16)|(buf[11]<<24);
		header_len = (uni_range_tbl[i].end_value - uni_range_tbl[i].start_value + 1) * 4 ;
		
		if (len < UNI_FONT_DB_LEN_MAX) {
			/* load all font to sdram */ 
			uni_range_tbl[i].uni_font_address = (INT8U*) gp_malloc(len);
		}
		else if (header_len < UNI_FONT_DB_HEADER_MAX) {
			/* just only load font header to sdram */ 
			uni_range_tbl[i].uni_font_address = (INT8U*) gp_malloc(header_len);
			if (uni_range_tbl[i].uni_font_address) {
				uni_range_tbl[i].font_header_only = 1;
			}
		}
		
		if (uni_range_tbl[i].font_header_only) {
			nv_read(fontdb_hd, (INT32U) uni_range_tbl[i].uni_font_address, header_len);
			nv_lseek(fontdb_hd,len-header_len,SEEK_CUR);
		}
		else if (uni_range_tbl[i].uni_font_address) {
			nv_read(fontdb_hd, (INT32U) uni_range_tbl[i].uni_font_address, len);
		}
		else {
			nv_lseek(fontdb_hd,len,SEEK_CUR);
		}
	}	
	
	/* sort */
    for(i=total_fonts-1;i>=0;i--) {
        for (j=0;j<=i-1;j++){ 
            if (uni_range_tbl[j].start_value > uni_range_tbl[j+1].start_value) {
                gp_memcpy((INT8S*)&tmp_uni_rabge_tbl,(INT8S*)&uni_range_tbl[j],sizeof(st_uni_range_tbl));
                gp_memcpy((INT8S*)&uni_range_tbl[j],(INT8S*)&uni_range_tbl[j+1],sizeof(st_uni_range_tbl));
                gp_memcpy((INT8S*)&uni_range_tbl[j+1],(INT8S*)&tmp_uni_rabge_tbl,sizeof(st_uni_range_tbl));
            }
        }
    }
	
}

void ap_umi_uni_string_draw(GraphicBitmap *bitmap,INT8S *strings,GraphicCoordinates *coordinates,INT8U ascii_font_type)
{
	INT16U chart;
	INT8U  *str;

	str = (INT8U*)strings;
	while(1) {
		chart = (INT16U)(*str | (*(str+1)) << 8);
		if (chart == 0) {
			break;
		}
		ap_umi_uni_char_draw(bitmap,chart,coordinates,ascii_font_type);
		str += 2;
	}
}

void ap_umi_uni_char_draw(GraphicBitmap *bitmap,unsigned short uni_code,GraphicCoordinates *coordinates,INT8U ascii_font_type)
{
	INT16U pos_x = 0,pos_y = 0;
	FontTableAttribute font;
	
	pos_x = coordinates ->start_x;
	pos_y = coordinates ->start_y;
	
	font.font_content = 0;
	
	if (uni_code < 0x80) {
		cl1_graphic_font_get(uni_code,&font,LCD_EN,ascii_font_type);
	}
	else {
		if (ap_umi_lang_font_get(uni_code,(t_FONT_TABLE_STRUCT*) &font) == STATUS_FAIL) {
			cl1_graphic_font_get('_',&font,LCD_EN,ascii_font_type); /* load font fail */
		}	
	}
	
	if (font.font_content == 0) {
		return;
	}
	
	if(((pos_x+font.font_width) >bitmap->bitmap_extend_x_size)||((pos_y+font.font_height) >bitmap->bitmap_extend_y_size))
	{	
		if(!(uni_code >=0x0030 && uni_code <= 0x0039)) {
			gp_free((void *)font.font_content);	
		}
		return ;
	}

	GraphicPutChar(bitmap,coordinates,&font);

	coordinates ->start_x += font.font_width;
	coordinates ->start_y = coordinates ->start_y;
	
	if(!(uni_code >=0x0030 && uni_code <= 0x0039)) {
		gp_free((void *)font.font_content);	//Neal added
	}
}

INT32S ap_umi_lang_font_get(INT16U uni_code, t_FONT_TABLE_STRUCT *font_table)
{
	INT32U offset, len, font_offset;
	INT32U *input_buffer;
	INT32S fontdb_idx;
	INT8U  header[8];
	INT8U  *font_addr;
	
	if (!font_table || !uni_range_tbl) {
		return STATUS_FAIL;
	}
	
	fontdb_idx = ap_umi_lang_uni_tbl_search(uni_code);
	
	if (fontdb_idx == -1) {
		return STATUS_FAIL;
	}
	
	font_addr = uni_range_tbl[fontdb_idx].uni_font_address;
	if (font_addr) { /* load from sdram */
		offset = (uni_code - uni_range_tbl[fontdb_idx].start_value)*4;
		gp_memcpy((INT8S*)header,(INT8S*)(font_addr+offset),4);
	}
	else { /* load from nvram */
		offset = uni_range_tbl[fontdb_idx].base_addr + (uni_code - uni_range_tbl[fontdb_idx].start_value)*4;
		nv_lseek(fontdb_hd, offset, SEEK_SET);
		if (nv_read(fontdb_hd, (INT32U) header, 4)) {
			return STATUS_FAIL;
		}
	}
	
	font_table->font_width = header[0];
	font_table->font_height = uni_range_tbl[fontdb_idx].font_height;
	font_table->bytes_per_line = uni_range_tbl[fontdb_idx].font_byte_per_line;
	font_offset = ((INT32U)header[1])|((INT32U)header[2]<<8)|((INT32U)header[3]<<16);
	if (font_offset == 0) {
		return STATUS_FAIL;
	}
	
	len = font_table->font_height*font_table->bytes_per_line;
	input_buffer = (INT32U *) gp_malloc(len);
	if (!input_buffer) {
		DBG_PRINT("Failed to allocate input_buffer in resource_font_load()\r\n");
		return STATUS_FAIL;
	}
	
	if (font_addr && uni_range_tbl[fontdb_idx].font_header_only == 0) {
		ap_umi_lang_font_data_adj1(font_table->font_height,font_table->bytes_per_line,font_addr+font_offset,(INT8U*)input_buffer);
	}
	else {
		font_offset += uni_range_tbl[fontdb_idx].base_addr;
		nv_lseek(fontdb_hd, font_offset, SEEK_SET);
		if (nv_read(fontdb_hd, (INT32U) input_buffer, len)) {
			gp_free(input_buffer);
	    	DBG_PRINT("Failed to read font data in resource_music_font_load()\r\n");
			return STATUS_FAIL;
		}
		ap_umi_lang_font_data_adj(font_table->font_height,font_table->bytes_per_line,(INT8U*)input_buffer);
	}
	font_table->font_content = (INT32U) input_buffer;

	return STATUS_OK;
}

INT32U ap_umi_lang_strings_width_get_for_no_newline(INT8S *strings,INT8U ascii_font_type)
{
	INT16U chart;
	INT16U width = 0;
	INT8U  *str;

	str = (INT8U*)strings;
	while(1) {
		chart = (INT16U)(*str | (*(str+1)) << 8);
		if (chart == 0) {
			break;
		}
		width += ap_umi_lang_font_width_get(chart,ascii_font_type);
		str += 2;
	}
	
	return width;
}

INT32U ap_umi_lang_font_width_get(INT16U uni_code,INT8U ascii_font_type)
{
	INT32U offset;
	INT32U fontdb_idx;
	INT8U  header[4];
	INT8U  *font_addr;
	INT8U  width;
	
	if (uni_code < 0x80) {
		return cl1_graphic_font_width_get(uni_code,LCD_EN,ascii_font_type);
	}
	
	if (!uni_range_tbl) {
		return 0;
	}
	
	fontdb_idx = ap_umi_lang_uni_tbl_search(uni_code);
	
	if (fontdb_idx == -1) {
		return cl1_graphic_font_width_get('_',LCD_EN,ascii_font_type);
	}
	
	font_addr = uni_range_tbl[fontdb_idx].uni_font_address;
	if (font_addr) { /* load from sdram */
		offset = (uni_code - uni_range_tbl[fontdb_idx].start_value)*4;
		width = *(font_addr+offset);
		if (width == 0) {
		    return cl1_graphic_font_width_get('_',LCD_EN,ascii_font_type);
		}
		return width;  
	}
	else { /* load from nvram */
		offset = uni_range_tbl[fontdb_idx].base_addr + (uni_code - uni_range_tbl[fontdb_idx].start_value)*4;
		nv_lseek(fontdb_hd, offset, SEEK_SET);
		if (nv_read(fontdb_hd, (INT32U) header, 4)) {
			return cl1_graphic_font_width_get('_',LCD_EN,ascii_font_type);
		}
		if (header[0] == 0) {
		    return cl1_graphic_font_width_get('_',LCD_EN,ascii_font_type);
		}    
		return header[0];
	}
}

INT32U ap_umi_lang_strings_height_get(INT8S *strings,INT8U ascii_font_type)
{
	INT16U chart;
	INT16U height = 0;
	INT16U temp_h;
	INT8U  *str;

	str = (INT8U*)strings;
	while(1) {
		chart = (INT16U)(*str | (*(str+1)) << 8);
		if (chart == 0) {
			break;
		}
		temp_h = ap_umi_lang_font_height_get(chart,ascii_font_type);
		if (temp_h > height) {
			height = temp_h;
		}
		str += 2;
	}

	return height;
}

INT32U ap_umi_lang_font_height_get(INT16U uni_code,INT8U ascii_font_type)
{
	INT32U fontdb_idx;
	
	if (uni_code < 0x80) {
		return cl1_graphic_font_height_get(uni_code,LCD_EN,ascii_font_type);
	}
	
	if (!uni_range_tbl) {
		return 0;
	}
	
	fontdb_idx = ap_umi_lang_uni_tbl_search(uni_code);
	
	if (fontdb_idx == -1) {
		return cl1_graphic_font_width_get('_',LCD_EN,ascii_font_type);
	}
	
	return uni_range_tbl[fontdb_idx].font_height;
}

void ap_umi_lang_font_data_adj(INT8U height,INT8U byte_per_line,INT8U* buf)
{
	INT32S i;
	
	for (i=(height-2)*byte_per_line-1;i>=0;i--) {
		buf[i+3] = buf[i];
	}
		
	for (i=0;i<byte_per_line;i++) {
		buf[i] = 0;
		buf[height*byte_per_line-1-i] = 0;
	}
}

void ap_umi_lang_font_data_adj1(INT8U height,INT8U byte_per_line,INT8U *src_buf,INT8U *dest_buf)
{
	INT32S i;
	
	for (i=(height-2)*byte_per_line-1;i>=0;i--) {
		dest_buf[i+3] = src_buf[i];
	}
		
	for (i=0;i<byte_per_line;i++) {
		dest_buf[i] = 0;
		dest_buf[height*byte_per_line-1-i] = 0;
	}
}

INT32S ap_umi_lang_uni_tbl_search(INT16U uni_code)
{
	INT16S start = 0;
	INT16S mid = 0;
	INT16S last = 0;
	
	last = total_fonts - 1;
	/* binary search */
	for(mid=((start+last)/2);start<=last;mid=(start+last)/2) {
		if(uni_code > uni_range_tbl[mid].end_value) {
			start = mid + 1;
		}
		else if(uni_code < uni_range_tbl[mid].start_value) {
			last = mid - 1;
		}
		else {
			return (INT32U) mid;
		}
	}
	return -1;
}

#endif
