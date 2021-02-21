
std::optional<std::vector<uint8_t>> png_data2;
LPDIRECT3DTEXTURE9 textureClass::textureFromMemory(IDirect3DDevice9* device, std::string value) noexcept
{
    LPDIRECT3DTEXTURE9 my_texture = NULL;
    HRESULT res = D3DXCreateTextureFromFileInMemory(device, value.data(), value.size(), &my_texture);
    if (res == S_OK)
    {
        D3DSURFACE_DESC my_image_desc;
        my_texture->GetLevelDesc(0, &my_image_desc);

        return my_texture;
    }
    else
        return NULL;
}

std::optional<std::vector<uint8_t>> png_data;
LPDIRECT3DTEXTURE9 textureClass::textureFromFile(IDirect3DDevice9* device, int type) noexcept
{
    if(type == 1) {
        if (gui->isOpen() && config->misc.preview) {
            int steps = 1;
            if (changed) {
                changed = false;
                if (config->visuals.selectedSkin != 0) {
                    auto& selected_entry = config->skinChanger[config->visuals.selectedSkin];
                    selected_entry.itemIdIndex = config->visuals.selectedSkin;
                    const auto& kits = config->visuals.selectedSkin == 1 ? SkinChanger::getGloveKits() : SkinChanger::getSkinKits();
                    std::string data = gui->split(kits[selected_entry.paint_kit_vector_index].name, " | ")[1];
                    std::transform(data.begin(), data.end(), data.begin(),
                        [](unsigned char c) { return std::tolower(c); });

                    if (data.find(" ") != std::string::npos)
                        data = gui->split(data, " ")[0];

                    if (weaponNames != "" && data != "") {
                        c_vpk_archive pak01_archive;
                        if (pak01_archive.load("C:/Program Files (x86)/Steam/steamapps/common/Counter-Strike Global Offensive/csgo/pak01_dir.vpk"))
                        {
                            std::string weapon(weaponNames);
                            std::optional<c_vpk_entry> entry = pak01_archive.get_file(weapon, data);
                            if (entry.has_value())
                            {
                                png_data = entry.value().get_data();
                            }
                        }
                    }
                }
            }

            if (png_data.has_value())
            {
                LPDIRECT3DTEXTURE9 my_texture = NULL;
                HRESULT res = D3DXCreateTextureFromFileInMemory(device, (char*)png_data.value().data(), png_data.value().size(), &my_texture);
                if (res == S_OK)
                {
                    D3DSURFACE_DESC my_image_desc;
                    my_texture->GetLevelDesc(0, &my_image_desc);
                    textureWidth = (int)my_image_desc.Width;
                    textureHeight = (int)my_image_desc.Height;

                    return my_texture;
                }
            }
        }
    }
   
    return NULL;
}
