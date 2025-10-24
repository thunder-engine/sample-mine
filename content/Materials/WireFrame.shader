<shader version="14">
    <properties>
        <property name="scale" type="float"/>
        <property name="width" type="float"/>
        <property name="ortho" type="bool"/>
    </properties>
    <fragment><![CDATA[
#version 450 core

#include "ShaderLayout.h"

layout(location = 0) out vec4 rgb;

void main() {
    rgb = vec4(1.0);
}
]]></fragment>
    <pass lightModel="Unlit" wireFrame="true" type="Surface" twoSided="true">
        <blend dst="OneMinusSourceAlpha" src="SourceAlpha" op="Add"/>
        <depth comp="Less" write="false" test="true"/>
    </pass>
</shader>
