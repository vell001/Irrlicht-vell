// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIImage.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"

namespace irr
{
namespace gui
{


//! constructor
CGUIImage::CGUIImage(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
: IGUIImage(environment, parent, id, rectangle), Texture(0), Color(255,255,255,255),
	UseAlphaChannel(false), ScaleImage(false), DrawBounds(0.f, 0.f, 1.f, 1.f), DrawBackground(true)
{
	#ifdef _DEBUG
	setDebugName("CGUIImage");
	#endif
}


//! destructor
CGUIImage::~CGUIImage()
{
	if (Texture)
		Texture->drop();
}


//! sets an image
void CGUIImage::setImage(video::ITexture* image)
{
	if (image == Texture)
		return;

	if (Texture)
		Texture->drop();

	Texture = image;

	if (Texture)
		Texture->grab();
}

//! Gets the image texture
video::ITexture* CGUIImage::getImage() const
{
	return Texture;
}

//! sets the color of the image
void CGUIImage::setColor(video::SColor color)
{
	Color = color;
}

//! Gets the color of the image
video::SColor CGUIImage::getColor() const
{
	return Color;
}

//! draws the element and its children
void CGUIImage::draw()
{
	if (!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();
	video::IVideoDriver* driver = Environment->getVideoDriver();

	if (Texture)
	{
		core::rect<s32> sourceRect(SourceRect);
		if (sourceRect.getWidth() == 0 || sourceRect.getHeight() == 0)
		{
			sourceRect = core::rect<s32>(core::dimension2di(Texture->getSize()));
		}

		if (ScaleImage)
		{
			const video::SColor Colors[] = {Color,Color,Color,Color};

			core::rect<s32> clippingRect(AbsoluteClippingRect);
			checkBounds(clippingRect);

			driver->draw2DImage(Texture, AbsoluteRect, sourceRect,
				&clippingRect, Colors, UseAlphaChannel);
		}
		else
		{
			core::rect<s32> clippingRect(AbsoluteRect.UpperLeftCorner, sourceRect.getSize());
			checkBounds(clippingRect);
			clippingRect.clipAgainst(AbsoluteClippingRect);

			driver->draw2DImage(Texture, AbsoluteRect.UpperLeftCorner, sourceRect,
				&clippingRect, Color, UseAlphaChannel);
		}
	}
	else if ( DrawBackground )
	{
		core::rect<s32> clippingRect(AbsoluteClippingRect);
		checkBounds(clippingRect);

		skin->draw2DRectangle(this, skin->getColor(EGDC_3D_DARK_SHADOW), AbsoluteRect, &clippingRect);
	}

	IGUIElement::draw();
}


//! sets if the image should use its alpha channel to draw itself
void CGUIImage::setUseAlphaChannel(bool use)
{
	UseAlphaChannel = use;
}


//! sets if the image should use its alpha channel to draw itself
void CGUIImage::setScaleImage(bool scale)
{
	ScaleImage = scale;
}


//! Returns true if the image is scaled to fit, false if not
bool CGUIImage::isImageScaled() const
{
	return ScaleImage;
}

//! Returns true if the image is using the alpha channel, false if not
bool CGUIImage::isAlphaChannelUsed() const
{
	return UseAlphaChannel;
}

//! Sets the source rectangle of the image. By default the full image is used.
void CGUIImage::setSourceRect(const core::rect<s32>& sourceRect)
{
	SourceRect = sourceRect;
}

//! Returns the customized source rectangle of the image to be used.
core::rect<s32> CGUIImage::getSourceRect() const
{
	return SourceRect;
}

//! Restrict target drawing-area.
void CGUIImage::setDrawBounds(const core::rect<f32>& drawBoundUVs)
{
	DrawBounds = drawBoundUVs;
	DrawBounds.UpperLeftCorner.X = core::clamp(DrawBounds.UpperLeftCorner.X, 0.f, 1.f);
	DrawBounds.UpperLeftCorner.Y = core::clamp(DrawBounds.UpperLeftCorner.Y, 0.f, 1.f);
	DrawBounds.LowerRightCorner.X = core::clamp(DrawBounds.LowerRightCorner.X, 0.f, 1.f);
	DrawBounds.LowerRightCorner.X = core::clamp(DrawBounds.LowerRightCorner.X, 0.f, 1.f);
	if ( DrawBounds.UpperLeftCorner.X > DrawBounds.LowerRightCorner.X )
		DrawBounds.UpperLeftCorner.X = DrawBounds.LowerRightCorner.X;
	if ( DrawBounds.UpperLeftCorner.Y > DrawBounds.LowerRightCorner.Y )
		DrawBounds.UpperLeftCorner.Y = DrawBounds.LowerRightCorner.Y;
}

//! Get target drawing-area restrictions.
core::rect<f32> CGUIImage::getDrawBounds() const
{
	return DrawBounds;
}

//! Writes attributes of the element.
void CGUIImage::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const
{
	IGUIImage::serializeAttributes(out,options);

	out->addTexture	("Texture", Texture);
	out->addBool	("UseAlphaChannel", UseAlphaChannel);
	out->addColor	("Color", Color);
	out->addBool	("ScaleImage", ScaleImage);
	out->addRect 	("SourceRect", SourceRect);
	out->addFloat   ("DrawBoundsX1", DrawBounds.UpperLeftCorner.X);
	out->addFloat   ("DrawBoundsY1", DrawBounds.UpperLeftCorner.Y);
	out->addFloat   ("DrawBoundsX2", DrawBounds.LowerRightCorner.X);
	out->addFloat   ("DrawBoundsY2", DrawBounds.LowerRightCorner.Y);
	out->addBool    ("DrawBackground", DrawBackground);
}


//! Reads attributes of the element
void CGUIImage::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
{
	IGUIImage::deserializeAttributes(in,options);

	setImage(in->getAttributeAsTexture("Texture", Texture));
	setUseAlphaChannel(in->getAttributeAsBool("UseAlphaChannel", UseAlphaChannel));
	setColor(in->getAttributeAsColor("Color", Color));
	setScaleImage(in->getAttributeAsBool("ScaleImage", ScaleImage));
	setSourceRect(in->getAttributeAsRect("SourceRect", SourceRect));

	DrawBounds.UpperLeftCorner.X = in->getAttributeAsFloat("DrawBoundsX1", DrawBounds.UpperLeftCorner.X);
	DrawBounds.UpperLeftCorner.Y = in->getAttributeAsFloat("DrawBoundsY1", DrawBounds.UpperLeftCorner.Y);
	DrawBounds.LowerRightCorner.X = in->getAttributeAsFloat("DrawBoundsX2", DrawBounds.LowerRightCorner.X);
	DrawBounds.LowerRightCorner.Y = in->getAttributeAsFloat("DrawBoundsY2", DrawBounds.LowerRightCorner.Y);
	setDrawBounds(DrawBounds);

	setDrawBackground(in->getAttributeAsBool("DrawBackground", DrawBackground));
}


} // end namespace gui
} // end namespace irr


#endif // _IRR_COMPILE_WITH_GUI_

