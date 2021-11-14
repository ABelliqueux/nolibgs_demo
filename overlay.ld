__heap_base = MAX(__ovly0_end, __ovly3_end);

SECTIONS {		
	OVERLAY __bss_end : NOCROSSREFS SUBALIGN(4)
	{
		.ovlyload
        {
            load_all_overlays_here = .;
        }
		.ovly0
			{			
				KEEP(OVL/hello_poly/*.o(.text))
				__ovly0_ctor = .;
				KEEP(OVL/hello_poly/*.o(.text.startup._GLOBAL__*))
				KEEP(OVL/hello_poly/*.o(.text.*))
				KEEP(OVL/hello_poly/*.o(.rodata*))
				KEEP(OVL/hello_poly/*.o(.sdata*))
				KEEP(OVL/hello_poly/*.o(.data*))
				KEEP(OVL/hello_poly/*.o(.sbss*))
				KEEP(OVL/hello_poly/*.o(.bss*))
				KEEP(OVL/hello_poly/*.o(.ctors))
				
				. = ALIGN(4);
				__ovly0_end = .;			
			}

			.ovly1
			{
				KEEP(OVL/hello_tile/*.o(.text))
				__ovly1_ctor = .;
				KEEP(OVL/hello_tile/*.o(.text.startup._GLOBAL__*))
				KEEP(OVL/hello_tile/*.o(.text.*))
				KEEP(OVL/hello_tile/*.o(.rodata*))
				KEEP(OVL/hello_tile/*.o(.sdata*))
				KEEP(OVL/hello_tile/*.o(.data*))
				KEEP(OVL/hello_tile/*.o(.sbss*))
				KEEP(OVL/hello_tile/*.o(.bss*))
				KEEP(OVL/hello_tile/*.o(.ctors))
				
				. = ALIGN(4);
				__ovly1_end = .;			
			}
		.ovly2
			{
				KEEP(OVL/hello_ovl_world/*.o(.text))
				__ovly2_ctor = .;
				KEEP(OVL/hello_ovl_world/*.o(.text.startup._GLOBAL__*))
				KEEP(OVL/hello_ovl_world/*.o(.text.*))
				KEEP(OVL/hello_ovl_world/*.o(.rodata*))
				KEEP(OVL/hello_ovl_world/*.o(.sdata*))
				KEEP(OVL/hello_ovl_world/*.o(.data*))
				KEEP(OVL/hello_ovl_world/*.o(.sbss*))
				KEEP(OVL/hello_ovl_world/*.o(.bss*))
				KEEP(OVL/hello_ovl_world/*.o(.ctors))
				
				. = ALIGN(4);
				__ovly2_end = .;			
			}
		.ovly3
			{
				KEEP(OVL/hello_sprt/*.o(.text))
				__ovly3_ctor = .;
				KEEP(OVL/hello_sprt/*.o(.text.startup._GLOBAL__*))
				KEEP(OVL/hello_sprt/*.o(.text.*))
				KEEP(OVL/hello_sprt/*.o(.rodata*))
				KEEP(OVL/hello_sprt/*.o(.sdata*))
				KEEP(OVL/hello_sprt/*.o(.data*))
				KEEP(OVL/hello_sprt/*.o(.sbss*))
				KEEP(OVL/hello_sprt/*.o(.bss*))
				KEEP(OVL/hello_sprt/*.o(.ctors))
				
				KEEP(OVL/hello_sprt/TIM/*.o(.text))
				__ovly3_ctor = .;
				KEEP(OVL/hello_sprt/TIM/*.o(.text.startup._GLOBAL__*))
				KEEP(OVL/hello_sprt/TIM/*.o(.text.*))
				KEEP(OVL/hello_sprt/TIM/*.o(.rodata*))
				KEEP(OVL/hello_sprt/TIM/*.o(.sdata*))
				KEEP(OVL/hello_sprt/TIM/*.o(.data*))
				KEEP(OVL/hello_sprt/TIM/*.o(.sbss*))
				KEEP(OVL/hello_sprt/TIM/*.o(.bss*))
				KEEP(OVL/hello_sprt/TIM/*.o(.ctors))
				
				. = ALIGN(4);
				__ovly3_end = .;			
			}
		.ovly4
			{
				KEEP(OVL/hello_cubetex/*.o(.text))
				__ovly4_ctor = .;
				KEEP(OVL/hello_cubetex/*.o(.text.startup._GLOBAL__*))
				KEEP(OVL/hello_cubetex/*.o(.text.*))
				KEEP(OVL/hello_cubetex/*.o(.rodata*))
				KEEP(OVL/hello_cubetex/*.o(.sdata*))
				KEEP(OVL/hello_cubetex/*.o(.data*))
				KEEP(OVL/hello_cubetex/*.o(.sbss*))
				KEEP(OVL/hello_cubetex/*.o(.bss*))
				KEEP(OVL/hello_cubetex/*.o(.ctors))
				
				KEEP(OVL/hello_cubetex/TIM/*.o(.text))
				__ovly4_ctor = .;
				KEEP(OVL/hello_cubetex/TIM/*.o(.text.startup._GLOBAL__*))
				KEEP(OVL/hello_cubetex/TIM/*.o(.text.*))
				KEEP(OVL/hello_cubetex/TIM/*.o(.rodata*))
				KEEP(OVL/hello_cubetex/TIM/*.o(.sdata*))
				KEEP(OVL/hello_cubetex/TIM/*.o(.data*))
				KEEP(OVL/hello_cubetex/TIM/*.o(.sbss*))
				KEEP(OVL/hello_cubetex/TIM/*.o(.bss*))
				KEEP(OVL/hello_cubetex/TIM/*.o(.ctors))
				
				. = ALIGN(4);
				__ovly4_end = .;			
			}
		.ovly5
			{
				KEEP(OVL/hello_light/*.o(.text))
				__ovly5_ctor = .;
				KEEP(OVL/hello_light/*.o(.text.startup._GLOBAL__*))
				KEEP(OVL/hello_light/*.o(.text.*))
				KEEP(OVL/hello_light/*.o(.rodata*))
				KEEP(OVL/hello_light/*.o(.sdata*))
				KEEP(OVL/hello_light/*.o(.data*))
				KEEP(OVL/hello_light/*.o(.sbss*))
				KEEP(OVL/hello_light/*.o(.bss*))
				KEEP(OVL/hello_light/*.o(.ctors))
				
				. = ALIGN(4);
				__ovly5_end = .;			
			}
		.ovly6
			{
				KEEP(OVL/hello_pad/*.o(.text))
				__ovly6_ctor = .;
				KEEP(OVL/hello_pad/*.o(.text.startup._GLOBAL__*))
				KEEP(OVL/hello_pad/*.o(.text.*))
				KEEP(OVL/hello_pad/*.o(.rodata*))
				KEEP(OVL/hello_pad/*.o(.sdata*))
				KEEP(OVL/hello_pad/*.o(.data*))
				KEEP(OVL/hello_pad/*.o(.sbss*))
				KEEP(OVL/hello_pad/*.o(.bss*))
				KEEP(OVL/hello_pad/*.o(.ctors))
				
				. = ALIGN(4);
				__ovly6_end = .;			
			}
	}
}
