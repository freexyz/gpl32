#include "fsystem.h"

//��ʼ��unicodeת������
static short  viet_init_nls(void)
{
	return 0;
}

//�˳�unicodeת������
static short viet_exit_nls(void)
{
	return 0;
}

//��unicodeת��Ϊ���ر�����ַ�
//parameter:  unicode
//return:     oem character
//note:       if error, return 0
static unsigned short viet_uni2char(unsigned short uni)
{
	
	
	
	
	return uni;
}

//�ѱ��ر�����ַ�ת��Ϊunicode
//parameter:  oem character
//return:     unicode
//note:       if error, return 0
//����**rawstringָ��һ��byte�͵��ַ�����
//ע��ĳЩ�ַ���2��byte��ɣ���ת����ɺ���Ҫ��ָ��*rawstring����2
//����ַ�ֻ��1��byte��ɣ���ת��Ϊunicode��ֻҪ��*rawstring����1
static unsigned short viet_char2uni(unsigned char **rawstring)
{
	unsigned char *temp = *rawstring;
	
	*rawstring += 1;        //the point add 1
	
	
	
	
	return *temp;
}

const struct nls_table nls_viet_table = {
	"viet",
	0, 
	viet_init_nls,
	viet_exit_nls,
	viet_uni2char,
	viet_char2uni,
};

