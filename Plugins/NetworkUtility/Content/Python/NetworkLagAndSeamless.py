import unreal

# Variable de session
CURRENT_LAG = 500

def apply_lag(value):
    """Applique la valeur de lag choisie."""
    global CURRENT_LAG
    CURRENT_LAG = value
    
    world = unreal.EditorLevelLibrary.get_editor_world()
    if world:
        unreal.SystemLibrary.execute_console_command(world, f"NetEmulation.PktLag {value}")
        unreal.log(f"Réseau : Lag réglé sur {value}ms")
    
    # Rafraîchir pour mettre à jour le label du bouton principal
    main()

def main():
    global CURRENT_LAG
    
    # 1. Seamless Travel au démarrage
    world = unreal.EditorLevelLibrary.get_editor_world()
    if world:
        unreal.SystemLibrary.execute_console_command(world, "net.AllowPIESeamlessTravel True")

    # 2. Création du Menu
    menus = unreal.ToolMenus.get()
    toolbar = menus.extend_menu("LevelEditor.LevelEditorToolBar.User")
    if not toolbar: return

    # Création d'un bouton de type COMBO (déroulant)
    entry = unreal.ToolMenuEntry(
        name="NetworkLagCombo",
        type=unreal.MultiBlockType.TOOL_BAR_COMBO_BUTTON # Transforme le bouton en menu
    )
    entry.set_label(f"Lag: {CURRENT_LAG}ms")
    toolbar.add_menu_entry("Scripts", entry)

    # Création du sous-menu qui s'ouvre quand on clique
    sub_menu_name = "LevelEditor.LevelEditorToolBar.User.NetworkLagCombo"
    sub_menu = menus.register_menu(sub_menu_name, type=unreal.MultiBoxType.MENU)
    
    # On ajoute des options de valeurs prédéfinies
    options = [0, 100, 250, 500, 1000]
    
    for val in options:
        sec_entry = unreal.ToolMenuEntry(
            name=f"LagOption_{val}",
            type=unreal.MultiBlockType.MENU_ENTRY
        )
        sec_entry.set_label(f"Set PktLag to {val}ms")
        # Commande pour appeler la fonction avec la valeur choisie
        sec_entry.set_string_command(
            unreal.ToolMenuStringCommandType.PYTHON,
            "",
            f"import NetworkLagAndSeamless; NetworkLagAndSeamless.apply_lag({val})"
        )
        sub_menu.add_menu_entry("Options", sec_entry)

    menus.refresh_all_widgets()

if __name__ == "__main__":
    main()