#ifndef LIBS_H
#define LIBS_H

#define NULL (void*)0

             // ARGB             
#define ORANGE 0xffffa500
#define CYAN   0xff00ffff
#define RED    0xffff0000
#define GREEN  0xff00ff00
#define BLUE   0xff0000ff
#define GRAY   0xff888888
#define WHITE  0xffffffff
#define BLACK  0xff000000

EFI_HANDLE ImageHandle;
EFI_SYSTEM_TABLE* SystemTable;
EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
EFI_GRAPHICS_OUTPUT_BLT_PIXEL GraphicsColor;
EFI_STATUS Status;
EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *Volume;

void SetTextPosition(UINT32 Col, UINT32 Row)
{
    SystemTable->ConOut->SetCursorPosition(SystemTable->ConOut, Col, Row);
}

void ResetScreen()
{
    SystemTable->ConOut->Reset(SystemTable->ConOut, 1);
}

void ClearScreen()
{
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
}

void SetColor(UINTN Attribute)
{
    SystemTable->ConOut->SetAttribute(SystemTable->ConOut, Attribute);
}

void Print(CHAR16* str)
{
    SystemTable->ConOut->OutputString(SystemTable->ConOut, str);
}

void HitAnyKey()
{
    SystemTable->ConIn->Reset(SystemTable->ConIn, 1);

    EFI_INPUT_KEY Key;
	while((SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key)) == EFI_NOT_READY);
}

void SetGraphicsColor(UINT32 color)
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL GColor;
    GColor.Reserved = color >> 24;
    GColor.Red      = color >> 16;
    GColor.Green    = color >> 8;
    GColor.Blue     = color;
    GraphicsColor = GColor;
}

void SetPixel(UINT32 xPos, UINT32 yPos)
{
    gop->Blt(gop, &GraphicsColor, 0, 0, 0, xPos, yPos, 1, 1, 0);
}

void CreateFilledBox(UINT32 xPos, UINT32 yPos, UINT32 w, UINT32 h, UINT32 color)
{
    SetGraphicsColor(color);
    gop->Blt(gop, &GraphicsColor, 0, 0, 0, xPos, yPos, w, h, 0);
}

void COLD_REBOOT()
{
    SystemTable->RuntimeServices->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, 0);
}

void WARM_REBOOT()
{
    SystemTable->RuntimeServices->ResetSystem(EfiResetWarm, EFI_SUCCESS, 0, 0);
}

void SHUTDOWN()
{
    SystemTable->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, 0);
}

void InitializeGOP()
{
    Print(L"\r\n\r\nLoading Graphics Output Protocol ... ");
    Status = SystemTable->BootServices->LocateProtocol(&EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID, 0, (void**)&gop);
    Print(CheckStandardEFIError(Status));
    if(Status == EFI_SUCCESS)
    {
        Print(L"Loading graphics...");
        CreateFilledBox(50, 50, 100, 200, ORANGE);
        CreateFilledBox(60, 60, 80, 30, RED);
        
        SetGraphicsColor(BLACK);
        SetPixel(65, 65);
        
        SetColor(EFI_YELLOW);
        SetTextPosition(3, 8);
        Print(L"\r\nWe have Graphics !!\r\n\r\n");
    }
}

void InitializeFILESYSTEM()
{
    SetColor(EFI_BROWN);
    Print(L"LoadedImage ... ");
    EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
    Status = SystemTable->BootServices->HandleProtocol(ImageHandle, &EFI_LOADED_IMAGE_PROTOCOL_GUID, (void**)&LoadedImage);
    SetColor(EFI_CYAN);
    Print(CheckStandardEFIError(Status));
    
    SetColor(EFI_BROWN);
    Print(L"DevicePath ... ");
    EFI_DEVICE_PATH_PROTOCOL *DevicePath;
    Status = SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &EFI_DEVICE_PATH_PROTOCOL_GUID, (void**)&DevicePath);
    SetColor(EFI_CYAN);
    Print(CheckStandardEFIError(Status));
    
    SetColor(EFI_BROWN);
    Print(L"Volume ... ");
    Status = SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID, (void**)&Volume);
    SetColor(EFI_CYAN);
    Print(CheckStandardEFIError(Status));
}

EFI_FILE_PROTOCOL* openFile(CHAR16* FileName)
{
    SetColor(EFI_BROWN);
    Print(L"RootFS ... ");
    EFI_FILE_PROTOCOL* RootFS;
    Status = Volume->OpenVolume(Volume, &RootFS);
    SetColor(EFI_CYAN);
    Print(CheckStandardEFIError(Status));
    
    SetColor(EFI_BROWN);
    Print(L"Opening File ... ");
    EFI_FILE_PROTOCOL* FileHandle = NULL;
    Status = RootFS->Open(RootFS, &FileHandle, FileName, 0x0000000000000001, 0);
    SetColor(EFI_CYAN);
    Print(CheckStandardEFIError(Status));
    
    return FileHandle;
}

void closeFile(EFI_FILE_PROTOCOL* FileHandle)
{
    SetColor(EFI_BROWN);
    Print(L"Closing File ... ");
    Status = FileHandle->Close(FileHandle);
    SetColor(EFI_CYAN);
    Print(CheckStandardEFIError(Status));
}

typedef struct PSF1_FONT
{
    UINT8        Signature[2];
    UINT8        mode;
    UINT8        charsize;
    char*        glyphBuffer[256];
} PSF1_FONT;

PSF1_FONT* LinuxFont;

void GraphicChar(char chr, unsigned int xOff, unsigned int yOff)
{
    char* fontPtr = (char*)LinuxFont->glyphBuffer + (chr * LinuxFont->charsize) - 3;
    for (unsigned long y = yOff; y < yOff + 16; y++){
        for (unsigned long x = xOff; x < xOff+8; x++){
            if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0){
                   gop->Blt(gop, &GraphicsColor, 0, 0, 0, x, y, 1, 1, 0);
                }
        }
        fontPtr++;
    }
}

typedef struct GCP{
    unsigned int X;
    unsigned int Y;
} GCP;

GCP GraphicCursorPosition;

void SetGraphicCursorPosition(unsigned int x, unsigned int y)
{
    GraphicCursorPosition.X = x;
    GraphicCursorPosition.Y = y;
}

void Printgraphictext(const char* str)
{
    char* chr = (char*)str;
    while(*chr != 0){
        GraphicChar(*chr, GraphicCursorPosition.X, GraphicCursorPosition.Y);
        GraphicCursorPosition.X+=8;
        if(GraphicCursorPosition.X + 8 > gop->Mode->Info->HorizontalResolution)
        {
            GraphicCursorPosition.X = 0;
            GraphicCursorPosition.Y += 16;
        }
        chr++;
    }
}

void TestFont()
{
    EFI_FILE_PROTOCOL* font = openFile(L"zap-light16.psf");

    UINTN lfsize = sizeof(PSF1_FONT);
    SetColor(EFI_BROWN);
    Print(L"AllocatingPool ... ");
    Status = SystemTable->BootServices->AllocatePool(EfiLoaderData, lfsize, (void**)&LinuxFont);
    SetColor(EFI_CYAN);
    Print(CheckStandardEFIError(Status));

    SetColor(EFI_BROWN);
    Print(L"Reading File ... ");
    Status = font->Read(font, &lfsize, LinuxFont);
    SetColor(EFI_CYAN);
    Print(CheckStandardEFIError(Status));
        
    closeFile(font);
    
    if(LinuxFont == NULL)
    {
        Print(L"Font Not Found\r\n");
    } else {
        Print(L"Font Found.");
        SetGraphicsColor(ORANGE);
        SetGraphicCursorPosition(250, 53);
        Printgraphictext("This is a Graphics text-string");
    }
}

#define DECIMAL 10
#define HEX     16

void itoa(unsigned long int n, CHAR16 *buffer, int basenumber)
{
	unsigned long int hold;
	int i, j;
	hold = n;
	i = 0;
	
	do{
		hold = n % basenumber;
		buffer[i++] = (hold < 10) ? (hold + '0') : (hold + 'a' - 10);
	} while(n /= basenumber);
	buffer[i--] = 0;
	
	for(j = 0; j < i; j++, i--)
	{
		hold = buffer[j];
		buffer[j] = buffer[i];
		buffer[i] = hold;
	}
}


#endif // LIBS_H