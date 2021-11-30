import type { ReactNode } from "react";

import { Skia } from "../../../skia";
import { useDeclaration } from "../../nodes/Declaration";
import type { AnimatedProps } from "../../processors/Animations/Animations";
import { isImageFilter } from "../../../skia/ImageFilter/ImageFilter";
import { isColorFilter } from "../../../skia/ColorFilter/ColorFilter";

export interface ColorFilterAsImageFilterProps {
  children: ReactNode | ReactNode[];
}

export const ColorFilterAsImageFilter = (
  props: AnimatedProps<ColorFilterAsImageFilterProps>
) => {
  const declaration = useDeclaration(props, (_, children) => {
    const [cf] = children.filter(isColorFilter);
    const [input] = children.filter(isImageFilter);
    return Skia.ImageFilter.MakeColorFilter(cf, input ?? null);
  });
  return <skDeclaration declaration={declaration} {...props} />;
};
