//
//  Shader.fsh
//  PorkyPinball
//
//  Created by Richard Thompson on 11/7/12.
//  Copyright (c) 2012 It-Just-Works Software, llc. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
