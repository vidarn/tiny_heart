import MetalKit

class ViewDelegate: NSObject, MTKViewDelegate {
    var input_state = InputState()
    var prev_input_state = InputState()
    
    init(view: MTKView) {

        satin_mtk_view = view;
        keys_down = NSMutableDictionary();
        satin_main()
        super.init()
    }
    
    func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize)
    {
        //print(size)
        //draw(in: view)
    }
    
    func draw(in view: MTKView) {
        #if os(iOS)
        #else
            let p = satin_mtk_view.window!.mouseLocationOutsideOfEventStream;
            let s = satin_mtk_view.frame.size;
            var inv_size = 1.0/s.height;
            var offset:[CGFloat] = [0.0, 0.0];
            if( s.width < s.height){
                inv_size = 1.0/s.width;
                offset[1] = (s.height - s.width)*0.5;
            }else{
                offset[0] = (s.width - s.height)*0.5;
            }
            input_state.mouse_x = Float((p.x - offset[0]) * inv_size);
            input_state.mouse_y = Float((p.y - offset[1]) * inv_size);
        #endif

        input_state.delta_x = input_state.mouse_x - prev_input_state.mouse_x;
        input_state.delta_y = input_state.mouse_y - prev_input_state.mouse_y;
        satin_update(input_state)
        prev_input_state = input_state;
        if(input_state.mouse_state == Int32(MOUSE_CLICKED.rawValue)){
            input_state.mouse_state = Int32(MOUSE_NOTHING.rawValue);
        }
        input_state.num_keys_typed = 0;
        /*if(input_state.mouse_state == Int32(MOUSE_DOUBLECLICKED.rawValue)){
            input_state.mouse_state = Int32(MOUSE_NOTHING.rawValue);
        }*/
    }
}
