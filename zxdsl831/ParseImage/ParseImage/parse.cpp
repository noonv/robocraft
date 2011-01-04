#include "parse.h"

firmware::firmware()
{
	firmware::bcm_tag = BCM_LENGTH;
	firmware::cfe=0;
	firmware::sqsh=0;
	firmware::kernel=0;
	firmware::model_name = NAME_LENGTH; // у D-link-ов
}

// считать заголовок
int bcmtag::read(FILE* fs)
{
	if(!fs){
		printf("[!] Error: empty FILE!\n");
		return -1;
	}

	// устанавливаемся на начало
	fseek(fs, 0, SEEK_SET);

	// считываем
	fread(bcmtag::tagVersion, sizeof(bcmtag::tagVersion), 1, fs);
	fread(bcmtag::signature_1, sizeof(bcmtag::signature_1), 1, fs);
	fread(bcmtag::signature_2, sizeof(bcmtag::signature_2), 1, fs);
	fread(bcmtag::chipId, sizeof(bcmtag::chipId), 1, fs);
	fread(bcmtag::boardId, sizeof(bcmtag::boardId), 1, fs);
	fread(bcmtag::bigEndian, sizeof(bcmtag::bigEndian), 1, fs);
	fread(bcmtag::totalImageLen, sizeof(bcmtag::totalImageLen), 1, fs);
	fread(bcmtag::cfeAddress, sizeof(bcmtag::cfeAddress), 1, fs);
	fread(bcmtag::cfeLen, sizeof(bcmtag::cfeLen), 1, fs);
	fread(bcmtag::rootfsAddress, sizeof(bcmtag::rootfsAddress), 1, fs);
	fread(bcmtag::rootfsLen, sizeof(bcmtag::rootfsLen), 1, fs);
	fread(bcmtag::kernelAddress, sizeof(bcmtag::kernelAddress), 1, fs);
	fread(bcmtag::kernelLen, sizeof(bcmtag::kernelLen), 1, fs);
	fread(bcmtag::dualImage, sizeof(bcmtag::dualImage), 1, fs);
	fread(bcmtag::inactiveLen, sizeof(bcmtag::inactiveLen), 1, fs);
	fread(bcmtag::reserved, sizeof(bcmtag::reserved), 1, fs);
	fread(bcmtag::imageValidationToken, sizeof(bcmtag::imageValidationToken), 1, fs);
	fread(bcmtag::tagValidationToken, sizeof(bcmtag::tagValidationToken), 1, fs);
}

// вывести информацию о заголовках
void bcmtag::show()
{
	int i=0;
	printf("[i] bcmtag:\n");
	printf("tagVersion: \t\t%s\n", bcmtag::tagVersion);
	printf("signature_1: \t\t%s\n", bcmtag::signature_1);
	printf("signature_2: \t\t%s\n", bcmtag::signature_2);
	printf("chipId: \t\t%s\n", bcmtag::chipId);
	printf("boardId: \t\t%s\n", bcmtag::boardId);
	printf("bigEndian: \t\t%d\n", atoi( (char*)bcmtag::bigEndian));
	printf("totalImageLen: \t\t%s\n", bcmtag::totalImageLen);
	printf("cfeAddress: \t\t%s\n", bcmtag::cfeAddress);
	printf("cfeLen: \t\t%s\n", bcmtag::cfeLen);
	printf("rootfsAddress: \t\t%s\n", bcmtag::rootfsAddress);
	printf("rootfsLen: \t\t%s\n", bcmtag::rootfsLen);
	printf("kernelAddress: \t\t%s\n", bcmtag::kernelAddress);
	printf("kernelLen: \t\t%s\n", bcmtag::kernelLen);
	printf("dualImage: \t\t%d\n", atoi((char*)bcmtag::dualImage));
	printf("inactiveLen: \t\t%d\n", atoi( (char*)bcmtag::inactiveLen));
	printf("reserved: \t\t%s\n", bcmtag::reserved);
	printf("imageValidationToken: \t");
	for(i=0; i<sizeof(bcmtag::imageValidationToken); i++){
		printf("%02X ", bcmtag::imageValidationToken[i]);
	}
	printf("\n");
	printf("tagValidationToken: \t");
	for(i=0; i<sizeof(bcmtag::tagValidationToken); i++){
		printf("%02X ", bcmtag::tagValidationToken[i]);
	}
	printf("\n");
	printf("\n");
}

// парсим прошивку
int parseFirmware(char* filename, bcmtag* tag)
{
	if(!filename){
		printf("[!] Error: filename is empty!\n");
		return -1;
	}
	if(!tag){
		printf("[!] Error: set bcmtag!\n");
		return -1;
	}

	printf("[i] Open file...\n");

	FILE* fs=0;
	fs = fopen(filename, "rb");

	if(!fs){
		printf("[!] Error: cant open file: %s!\n", filename);
		return -2;
	}

	// узнаем размер файла
	fseek(fs, 0, SEEK_END); // позицию в конец фала
	unsigned fileSize = ftell(fs); // получаем позицию

	printf("[i] file size: %d\n", fileSize);

	//считываем 256-байтный заголовок
	//fread(tag, 256, 1, fs);
	tag->read(fs);

	// показываем информацию
	tag->show();

	// заполняем данные прошивки
	firmware img;

	img.cfe = atoi( (char*)tag->cfeLen);
	img.sqsh = atoi( (char*)tag->rootfsLen);
	img.kernel = atoi( (char*)tag->kernelLen);

	bool isDlink = false;

	printf("[i] Ok. Size good for: ");

	if(img.bcm_tag+img.cfe+img.sqsh+img.kernel == fileSize){
		printf("Broadcom!\n");
		img.model_name = 0;
	}
	else if(img.bcm_tag+img.cfe+img.sqsh+img.kernel+img.model_name == fileSize){
		printf("D-link!\n");
		isDlink = true;
	}

	// создаём файлы для сохранения частей прошивки
	FILE* fbcm=0, *fcfe=0, *fsqsh=0, *fkernel=0, *fname=0;

	fbcm = fopen(bcm_file, "wb+");
	fcfe = fopen(cfe_file, "wb+");
	fsqsh = fopen(sqsh_file, "wb+");
	fkernel = fopen(kernel_file, "wb+");
	if(isDlink){
		fname = fopen(name_file, "wb+");
		if(!fname){
			printf("[!] Error: cant open file for writing: %s!\n", name_file);
		}
	}
	if(!fbcm || !fcfe || !fsqsh || !fkernel){
		printf("[!] Error: cant open file for writing: %s!\n");
		fclose(fs);
		return -3;
	}

	//
	// считываем файл в буфер
	//
	uchar* buf = new uchar [fileSize];
	if(!buf){
		printf("[!] Error: cant allocate memory!\n");
		fclose(fs);
		return -4;
	}
	// устанавливаемся на начало
	fseek(fs, 0, SEEK_SET);
	fread(buf, fileSize, 1, fs);
#if 1
	//
	// CRC
	//
	uint32_t crc;
	char file_crc[16], header_crc[16], image_crc[16], rootfs_crc[16], kernel_crc[16];
	char header_crc_read[16], image_crc_read[16], rootfs_crc_read[16], kernel_crc_read[16];
	int swap=0, goodcheck=0;

	// весь файл
	crc = CRC32_INIT_VALUE;
	crc = getCrc32((uint8_t *) (buf), (uint32_t) (fileSize), crc);
	sprintf(file_crc, "%08lX", (long unsigned int) crc);
	//goodcheck=(strcmp (image_crc, image_crc_read, 8) == 0);
	printf("[i] CRC file\t\t: 0x%s\n", file_crc);

	// заголовок
	crc = CRC32_INIT_VALUE;
	crc = getCrc32((uint8_t *) (buf), (uint32_t) (img.bcm_tag-TOKEN_LEN), crc);
	sprintf(header_crc, "%08lX", (long unsigned int) crc);
	//goodcheck=(strcmp (image_crc, image_crc_read, 8) == 0);
	printf("[i] CRC bcmtag\t\t: 0x%s\n", header_crc);

	// образ (cfe+sqsh+kernel)
	crc = CRC32_INIT_VALUE;
	crc = getCrc32((uint8_t *) (buf + TAG_LEN), (uint32_t) (img.cfe+img.sqsh+img.kernel), crc);
	sprintf(image_crc, "%08lX", (long unsigned int) crc);
	printf("[i] CRC image\t\t: 0x%s\n", image_crc);
	if(isDlink){
		crc = CRC32_INIT_VALUE;
		crc = getCrc32((uint8_t *) (buf + TAG_LEN), (uint32_t) (img.cfe+img.sqsh+img.kernel+img.model_name), crc);
		sprintf(image_crc, "%08lX", (long unsigned int) crc);
		printf("[i] D-link CRC image\t\t: 0x%s\n", image_crc);
	}

	// корневая файловая система: sqsh
	crc = CRC32_INIT_VALUE;
	crc = getCrc32((uint8_t *) (buf + img.bcm_tag + img.cfe), (uint32_t) (img.sqsh), crc);
	sprintf(rootfs_crc, "%08lX", (long unsigned int) crc);
	printf("[i] CRC sqsh\t\t: 0x%s\n", rootfs_crc);

	// ядро - kernel
	crc = CRC32_INIT_VALUE;
	crc = getCrc32((uint8_t *) (buf + img.bcm_tag + img.cfe + img.sqsh), (uint32_t) (img.kernel), crc);
	sprintf(kernel_crc, "%08lX", (long unsigned int) crc);
	printf("[i] CRC kernel\t\t: 0x%s\n", kernel_crc);
#endif

	// записываем файлы

	// bcm
	writePieceOfFile(fs, fbcm, 0, img.bcm_tag);
	fclose(fbcm);

	// cfe
	writePieceOfFile(fs, fcfe, img.bcm_tag, img.cfe);
	fclose(fcfe);

	// sqfs
	writePieceOfFile(fs, fsqsh, img.bcm_tag+img.cfe, img.sqsh);
	fclose(fsqsh);

	// kernel
	writePieceOfFile(fs, fkernel, img.bcm_tag+img.cfe+img.sqsh, img.kernel);
	fclose(fkernel);

	// если Dlink, то и name
	if(isDlink && fname){
		writePieceOfFile(fs, fname, img.bcm_tag+img.cfe+img.sqsh+img.kernel, img.model_name);
		fclose(fname);
	}

	if(buf){
		delete []buf;
		buf=0;
	}

	printf("[i] Close file.\n");
	fclose(fs);
	return 0;
}

// записать из файла в файл кусок данных
// src - источник
// dst - приёмник (должен быть открыт на запись!)
// begin_pos - начальная позиция
// length - длина
int writePieceOfFile(FILE* src, FILE* dst, uint begin_pos, uint length)
{
	if(!src || !dst){
		printf("[!] Error: empty FILE!\n");
		return -1;
	}

	// устанавливаемся на начальную позицию
	if(fseek(src, begin_pos, SEEK_SET)!=0){
		printf("[!] Error: set position (%d) in file!\n", begin_pos);
		return -2;
	}

	// устанавливаемся на начало
	fseek(dst, 0, SEEK_SET);

	uchar* buf = new uchar [BUF_SIZE];
	if(!buf){
		printf("[!] Error: cant allocate memory!\n");
		return -3;
	}

	uint i=0;
	size_t read=0, write=0;
	size_t for_read = BUF_SIZE;
	while(i<length){
		/**/
		// считываем файл блоками заданного размера
		if(length-i>BUF_SIZE){
			for_read = BUF_SIZE;
		}
		else{
			// оставшийся кусочек
			for_read = length-i;
		}
		/**/
		
		read = fread(buf, for_read, 1, src);
		i+=read*for_read;
		// записываем
		write = fwrite(buf, for_read, 1, dst);
	}

	if(buf){
		delete []buf;
		buf=0;
	}

	return 0;
}

// записать из массива в файл кусок данных
// src - источник
// dst - приёмник (должен быть открыт на запись!)
// begin_pos - начальная позиция
// length - длина
int writePieceOfData(uchar* src, FILE* dst, uint begin_pos, uint length)
{
	if(!src || !dst){
		printf("[!] Error: empty param!\n");
		return -1;
	}

	// устанавливаемся на начало
	fseek(dst, 0, SEEK_SET);

	uchar buf;
	uint size = begin_pos+length;
	size_t write=0;
	for(uint i=begin_pos; i<size; i++){
		buf = src[i];
		// записываем
		write = fwrite(&buf, sizeof(uchar), 1, dst);
	}

	return 0;
}


// получить размер файла
size_t getFileSize(FILE* fs)
{
	size_t size = 0;

	if(!fs){
		printf("[!] Error: empty FILE!\n");
		return 0;
	}

	// запоминаем текущее положение
	size_t pos = ftell(fs);

	// узнаем размер файла
	fseek(fs, 0, SEEK_END); // позицию в конец фала
	size = ftell(fs); // получаем позицию

	printf("[i] file size: %d\n", size);

	// устанавливаем предыдущее положение
	fseek(fs, pos, SEEK_SET);

	return size;
}

//============================================================

uint32_t getCrc32(uint8_t *pdata, uint32_t size, uint32_t crc)
{
    while (size-- > 0)
    {
        crc = (crc >> 8) ^ Crc32_table[(crc ^ *pdata++) & 0xff];
    }

    return crc;
}

uint32_t getCRC32String(uint8_t *pdata, uint32_t size, char* dst)
{
	if(!pdata){
		printf("[!] Error: src buffer is null!\n");
		return 0;
	}
	if(!dst){
		printf("[!] Error: buffer for CRC is null!\n");
		return 0;
	}

	uint32_t crc;

	crc = CRC32_INIT_VALUE;
	crc = getCrc32(pdata, size, crc);
	sprintf(dst, "%08lX", (long unsigned int) crc);

	return crc;
}

// записать 4-байта CRC в заданном месте буфера
void updateCRC(uint8_t *pdata, uint32_t start, uint32_t crc)
{
	if(!pdata){
		printf("[!] Error: src buffer is null!\n");
		return;
	}

	uint i= start;
	pdata[i++] = (uint8_t)(crc>>24);
	pdata[i++] = (uint8_t)(crc>>16 & 0xFF);
	pdata[i++] = (uint8_t)(crc>>8 & 0xFF);
	pdata[i++] = (uint8_t)(crc & 0xFF);
}

// записать данные о длине в заданном месте буфера
void updateLength(uint8_t *pdata, uint32_t start, uint32_t data, uint32_t size)
{
	if(!pdata){
		printf("[!] Error: src buffer is null!\n");
		return;
	}
	if(!size){
		printf("[!] Error: set size of buffer for write!\n");
		return;
	}

	uint32_t i=0, j=0;
	char* buf = new char[size];
	
	if(!buf){
		printf("[!] Error: cant allocate memory!\n");
		return;
	}

	for(i=0; i<size; i++){
		buf[i]=0;
	}

	sprintf(buf, "%d", (long unsigned int) data);

	for(j=0, i=start; j<size; j++, i++){
		pdata[i] = (uchar)buf[j];
	}

	if(buf){
		delete []buf;
		buf=0;
	}	
}