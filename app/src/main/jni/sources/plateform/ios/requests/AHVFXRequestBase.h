//
//  AHVFXRequestBase.h
//  AHVideoFX
//
//  Created by longxiang on 2019/3/25.
//  Copyright © 2019 AH. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface AHVFXRequestBase : NSObject

@property (nonatomic, assign) uint32_t Id;
@property (nonatomic, assign) uint32_t width;
@property (nonatomic, assign) uint32_t height;

- (void)prepareAtTime:(float)time;

@end

NS_ASSUME_NONNULL_END
