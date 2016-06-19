//
//  Shader.fsh
//  Starports
//
//  Created by Amine Rehioui on 2015-03-15.
//  Copyright (c) 2015 Frantic. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
