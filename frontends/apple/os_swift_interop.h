//
//  os_swift_interop.h
//  metal_test
//
//  Created by Vidar Nelson on 2019-10-18.
//  Copyright © 2019 Vidar Nelson. All rights reserved.
//

#ifndef os_swift_interop_h
#define os_swift_interop_h

#import <MetalKit/MetalKit.h>
extern MTKView *satin_mtk_view;
extern NSMutableDictionary *keys_down;

void satin_update(struct InputState input_state);

extern int satin_num_game_states;
extern void *satin_param;
extern struct GameState *satin_game_states;

#endif /* os_swift_interop_h */
