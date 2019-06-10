//
//  AHVFXRequestImage.h
//  AHVideoFXFramework
//
//  Created by longxiang on 2019/3/20.
//  Copyright © 2019 AH. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIImage.h>
#import "AHVFXRequestBase.h"

NS_ASSUME_NONNULL_BEGIN

@interface AHVFXRequestImage : AHVFXRequestBase

- (id)initWithImage:(UIImage *)image;
- (id)initWithImagePath:(NSString *)image_path;

@end

NS_ASSUME_NONNULL_END
