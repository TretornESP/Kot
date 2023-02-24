#include <kot-ui++/component.h>

#include <kot-ui++/pictures/picture.h>

#include <kot/uisd/srvs/storage.h>

namespace Ui {
    void PictureboxDraw(Picturebox_t* Picturebox){
        Picturebox->Cpnt->IsRedraw = true;
        switch(Picturebox->Type){
            case PictureboxType::_TGA:{
                switch(Picturebox->Style.Fit){
                    case PictureboxFit::PICTUREFIT:
                    {   
                        TGA_t* ImageRead = TGARead((TGAHeader_t*)Picturebox->Image);
                        TGA_t* ImageResize = TGAResize(ImageRead, Picturebox->Cpnt->Style->Currentwidth, Picturebox->Cpnt->Style->Currentheight, true);

                        ImageResize->x = Picturebox->Cpnt->Style->Currentwidth / 2 - ImageResize->Width / 2; + Picturebox->Cpnt->Style->Position.x + Picturebox->Cpnt->Style->Margin.Left;
                        ImageResize->y = Picturebox->Cpnt->Style->Currentheight / 2 - ImageResize->Height / 2 + Picturebox->Cpnt->Style->Position.y + Picturebox->Cpnt->Style->Margin.Top;

                        TGADraw(Picturebox->Cpnt->Framebuffer, ImageResize);

                        free(ImageResize);
                        free(ImageRead);
                        break;
                    }

                    case PictureboxFit::PICTUREFILL:
                    {   
                        TGA_t* ImageRead = TGARead((TGAHeader_t*)Picturebox->Image);
                        uint16_t _Width = ((TGAHeader_t*)Picturebox->Image)->Width, _Height = ((TGAHeader_t*)Picturebox->Image)->Height;

                        // permet de faire en sorte que l'image dépasse la taille du monitor
                        if(_Width > _Height) {
                            _Width = NULL;
                            _Height = Picturebox->Cpnt->Style->Currentheight;
                        } else{
                            _Width = Picturebox->Cpnt->Style->Currentwidth;
                            _Height = NULL;
                        }
                        
                        TGA_t* ImageResize = TGAResize(ImageRead, _Width, _Height, true);

                        uint16_t x = (ImageResize->Width - Picturebox->Cpnt->Style->Currentwidth) / 2;
                        uint16_t y = (ImageResize->Height - Picturebox->Cpnt->Style->Currentheight) / 2;

                        TGA_t* ImageCrop = TGACrop(ImageResize, Picturebox->Cpnt->Style->Currentwidth, Picturebox->Cpnt->Style->Currentheight, x, y);

                        ImageCrop->x = Picturebox->Cpnt->Style->Position.x + Picturebox->Cpnt->Style->Margin.Left;
                        ImageCrop->y = Picturebox->Cpnt->Style->Position.y + Picturebox->Cpnt->Style->Margin.Top; 

                        TGADraw(Picturebox->Cpnt->Framebuffer, ImageCrop);

                        free(ImageResize);
                        free(ImageCrop);
                        free(ImageRead);
                        break;
                    }

                    case PictureboxFit::PICTURECENTER:
                    {
                        TGA_t* ImageRead = TGARead((TGAHeader_t*)Picturebox->Image);
                        uint16_t x = 0, y = 0,
                            _Width = ((TGAHeader_t*)Picturebox->Image)->Width, _Height = ((TGAHeader_t*)Picturebox->Image)->Height;

                        if(_Width > Picturebox->Cpnt->Style->Currentwidth) {
                            x = (_Width - Picturebox->Cpnt->Style->Currentwidth) / 2;
                            _Width = Picturebox->Cpnt->Style->Currentwidth;
                        }
                        if(_Height > Picturebox->Cpnt->Style->Currentheight) {
                            y = (_Height - Picturebox->Cpnt->Style->Currentheight) / 2;
                            _Height = Picturebox->Cpnt->Style->Currentheight;
                        }

                        // si l'image dépasse le monitor alors on crop
                        if(((TGAHeader_t*)Picturebox->Image)->Width > Picturebox->Cpnt->Style->Currentwidth || ((TGAHeader_t*)Picturebox->Image)->Height > Picturebox->Cpnt->Style->Currentheight) {
                            TGA_t* ImageCrop = TGACrop(ImageRead, _Width, _Height, x, y);

                            ImageCrop->x = Picturebox->Cpnt->Style->Currentwidth / 2 - ImageCrop->Width / 2;
                            ImageCrop->y = Picturebox->Cpnt->Style->Currentheight / 2 - ImageCrop->Height / 2;

                            ImageCrop->x = Picturebox->Cpnt->Style->Position.x + Picturebox->Cpnt->Style->Margin.Left;
                            ImageCrop->y = Picturebox->Cpnt->Style->Position.y + Picturebox->Cpnt->Style->Margin.Top;

                            TGADraw(Picturebox->Cpnt->Framebuffer, ImageCrop);
                            
                            free(ImageCrop);
                        } else {
                            ((TGAHeader_t*)Picturebox->Image)->x = Picturebox->Cpnt->Style->Currentwidth / 2 - ((TGAHeader_t*)Picturebox->Image)->Width / 2;
                            ((TGAHeader_t*)Picturebox->Image)->y = Picturebox->Cpnt->Style->Currentheight / 2 - ((TGAHeader_t*)Picturebox->Image)->Height / 2;
                            
                            ImageRead->x = Picturebox->Cpnt->Style->Position.x + Picturebox->Cpnt->Style->Margin.Left;
                            ImageRead->y = Picturebox->Cpnt->Style->Position.y + Picturebox->Cpnt->Style->Margin.Top;

                            TGADraw(Picturebox->Cpnt->Framebuffer, ImageRead);
                        }

                        free(ImageRead);

                        break;
                    }

                    case PictureboxFit::PICTURESTRETCH:
                    {   
                        TGA_t* ImageRead = TGARead((TGAHeader_t*)Picturebox->Image);
                        TGA_t* ImageResize = TGAResize(ImageRead, Picturebox->Cpnt->Style->Currentwidth, Picturebox->Cpnt->Style->Currentheight);
                        
                        ImageResize->x = Picturebox->Cpnt->Style->Position.x + Picturebox->Cpnt->Style->Margin.Left;
                        ImageResize->y = Picturebox->Cpnt->Style->Position.y + Picturebox->Cpnt->Style->Margin.Top;

                        TGADraw(Picturebox->Cpnt->Framebuffer, ImageResize);

                        free(ImageResize);
                        free(ImageRead);
                        break;
                    }

                    default:
                        break;
                }
            }

            default:
                break;
        }
    }
    
    void PictureboxUpdate(Component* Cpnt){
        Picturebox_t* Picturebox = (Picturebox_t*)Cpnt->ExternalData;
        if(Cpnt->IsFramebufferUpdate){
            Cpnt->IsFramebufferUpdate = false;
            PictureboxDraw(Picturebox);
        }else if(Picturebox->IsDrawUpdate){
            Picturebox->IsDrawUpdate = false;
            PictureboxDraw(Picturebox);
        }else if(Cpnt->Parent->IsRedraw || Cpnt->DrawPosition.x != Cpnt->Style->Position.x || Cpnt->DrawPosition.y != Cpnt->Style->Position.y){
            PictureboxDraw(Picturebox);
        }
        Cpnt->AbsolutePosition = {.x = (int64_t)(Cpnt->Parent->AbsolutePosition.x + Cpnt->Style->Position.x + Cpnt->Style->Margin.Left), .y = (int64_t)(Cpnt->Parent->AbsolutePosition.y + Cpnt->Style->Position.y + Cpnt->Style->Margin.Top)};
    }

    void PictureboxMouseEvent(class Component* Cpnt, bool IsHover, int64_t RelativePositionX, int64_t RelativePositionY, int64_t PositionX, int64_t PositionY, int64_t ZValue, uint64_t Status){
        if(IsHover){
            Picturebox_t* Picturebox = (Picturebox_t*)Cpnt->ExternalData;
            if(Cpnt->UiCtx->FocusCpnt != Cpnt){
                if(Cpnt->UiCtx->FocusCpnt->MouseEvent){
                    Cpnt->UiCtx->FocusCpnt->MouseEvent(Cpnt->UiCtx->FocusCpnt, false, RelativePositionX, RelativePositionY, PositionX, PositionY, ZValue, Status);
                }
            }
            Cpnt->UiCtx->FocusCpnt = Picturebox->Cpnt;
        }
    }

    Picturebox_t* Picturebox(char* Path, PictureboxType Type, PictureboxStyle_t Style, Component* ParentCpnt) {
        file_t* ImageFile = fopen(Path, "rb");

        if(ImageFile == NULL)
            return NULL;

        fseek(ImageFile, 0, SEEK_END);
        size_t ImageFileSize = ftell(ImageFile);
        fseek(ImageFile, 0, SEEK_SET);

        uintptr_t Image = malloc(ImageFileSize);
        fread(Image, ImageFileSize, 1, ImageFile);

        uint16_t Width;
        uint16_t Height;
        switch(Type){
            case PictureboxType::_TGA:
                Width = ((TGAHeader_t*)Image)->Width;
                Height = ((TGAHeader_t*)Image)->Height;
                break;
            default:
                free(Image); 
                fclose(ImageFile);
                return NULL;
                break;
        }

        if(Width <= 0 || Height <= 0) { 
            free(Image); 
            fclose(ImageFile); 
            return NULL; 
        }

        Picturebox_t* Picturebox = (Picturebox_t*)malloc(sizeof(Picturebox_t));
        Picturebox->Type = Type;
        Picturebox->Image = Image;
        memcpy(&Picturebox->Style, &Style, sizeof(PictureboxStyle_t));
        Picturebox->Cpnt = new Component(Style.G, PictureboxUpdate, PictureboxMouseEvent, (uintptr_t)Picturebox, ParentCpnt, true, false);
        Picturebox->IsDrawUpdate = true;
        fclose(ImageFile);
        
        return Picturebox;
    }

}