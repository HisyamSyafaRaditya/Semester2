// TODO: 4. Implement fsMv function
void fsMv(struct file_metadata* src_metadata, struct file_metadata* dst_metadata, enum fs_return* status) {
    struct node_fs node_fs_buf;
    int i, src_match, dst_parent_exists;
    
    // Read node sectors
    readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
    readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER);
    
    // Find source file
    src_match = -1;
    for (i = 0; i < FS_MAX_NODE; i++) {
        if (strcmp(node_fs_buf.nodes[i].node_name, src_metadata->node_name) == 0 && 
            node_fs_buf.nodes[i].parent_index == src_metadata->parent_index) {
            src_match = i;
            break;
        }
    }
    
    if (src_match == -1) {
        *status = FS_R_NODE_NOT_FOUND;
        return;
    }
    
    // Check if source is a file (not directory)
    if (node_fs_buf.nodes[src_match].data_index == 0xFF) {
        *status = FS_R_TYPE_IS_DIRECTORY;
        return;
    }
    
    // Check if destination parent exists
    dst_parent_exists = 0;
    for (i = 0; i < FS_MAX_NODE; i++) {
        if (i == dst_metadata->parent_index) {
            dst_parent_exists = 1;
            break;
        }
    }
    
    if (!dst_parent_exists && dst_metadata->parent_index != 0xFF) {
        *status = FS_R_NODE_NOT_FOUND;
        return;
    }
    
    // Check if destination file already exists
    for (i = 0; i < FS_MAX_NODE; i++) {
        if (strcmp(node_fs_buf.nodes[i].node_name, dst_metadata->node_name) == 0 && 
            node_fs_buf.nodes[i].parent_index == dst_metadata->parent_index) {
            *status = FS_W_NODE_ALREADY_EXISTS;
            return;
        }
    }
    
    // Move file: update name and parent_index
    strcpy(node_fs_buf.nodes[src_match].node_name, dst_metadata->node_name);
    node_fs_buf.nodes[src_match].parent_index = dst_metadata->parent_index;
    
    // Write back to disk
    writeSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
    writeSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER);
    
    *status = FS_W_SUCCESS;
}

// TODO: 5. Implement fsCp function
void fsCp(struct file_metadata* src_metadata, struct file_metadata* dst_metadata, enum fs_return* status) {
    struct map_fs map_fs_buf;
    struct node_fs node_fs_buf;
    struct data_fs data_fs_buf;
    int i, j, src_match, dst_node_match, dst_data_match, emptySize;
    char temp_buffer[SECTOR_SIZE * FS_MAX_SECTOR];
    
    // Read all necessary sectors
    readSector(&map_fs_buf, FS_MAP_SECTOR_NUMBER);
    readSector(&data_fs_buf, FS_DATA_SECTOR_NUMBER);
    readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
    readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER);
    
    // Find source file
    src_match = -1;
    for (i = 0; i < FS_MAX_NODE; i++) {
        if (strcmp(node_fs_buf.nodes[i].node_name, src_metadata->node_name) == 0 && 
            node_fs_buf.nodes[i].parent_index == src_metadata->parent_index) {
            src_match = i;
            break;
        }
    }
    
    if (src_match == -1) {
        *status = FS_R_NODE_NOT_FOUND;
        return;
    }
    
    // Check if source is a file (not directory)
    if (node_fs_buf.nodes[src_match].data_index == 0xFF) {
        *status = FS_R_TYPE_IS_DIRECTORY;
        return;
    }
    
    // Check if destination file already exists
    for (i = 0; i < FS_MAX_NODE; i++) {
        if (strcmp(node_fs_buf.nodes[i].node_name, dst_metadata->node_name) == 0 && 
            node_fs_buf.nodes[i].parent_index == dst_metadata->parent_index) {
            *status = FS_W_NODE_ALREADY_EXISTS;
            return;
        }
    }
    
    // Find empty node for destination
    dst_node_match = -1;
    for (i = 0; i < FS_MAX_NODE; i++) {
        if (node_fs_buf.nodes[i].node_name[0] == '\0') {
            dst_node_match = i;
            break;
        }
    }
    
    if (dst_node_match == -1) {
        *status = FS_W_NO_FREE_NODE;
        return;
    }
    
    // Find empty data entry for destination
    dst_data_match = -1;
    for (i = 0; i < FS_MAX_DATA; i++) {
        if (data_fs_buf.datas[i].sectors[0] == 0x00) {
            dst_data_match = i;
            break;
        }
    }
    
    if (dst_data_match == -1) {
        *status = FS_W_NO_FREE_DATA;
        return;
    }
    
    // Count sectors used by source file
    int sectors_used = 0;
    for (i = 0; i < FS_MAX_SECTOR; i++) {
        if (data_fs_buf.datas[node_fs_buf.nodes[src_match].data_index].sectors[i] == 0x00) break;
        sectors_used++;
    }
    
    // Check available space
    emptySize = 0;
    for (i = 0; i < SECTOR_SIZE; i++) {
        if (map_fs_buf.is_used[i] == 0) emptySize++;
    }
    
    if (emptySize < sectors_used) {
        *status = FS_W_NOT_ENOUGH_SPACE;
        return;
    }
    
    // Read source file data
    for (i = 0; i < sectors_used; i++) {
        readSector(temp_buffer + i * SECTOR_SIZE, 
                   data_fs_buf.datas[node_fs_buf.nodes[src_match].data_index].sectors[i]);
    }
    
    // Create destination node
    strcpy(node_fs_buf.nodes[dst_node_match].node_name, dst_metadata->node_name);
    node_fs_buf.nodes[dst_node_match].parent_index = dst_metadata->parent_index;
    node_fs_buf.nodes[dst_node_match].data_index = dst_data_match;
    
    // Allocate new sectors and write data
    j = 0;
    for (i = 0; i < SECTOR_SIZE && j < sectors_used; i++) {
        if (!map_fs_buf.is_used[i]) {
            map_fs_buf.is_used[i] = 1;
            data_fs_buf.datas[dst_data_match].sectors[j] = i;
            writeSector(temp_buffer + j * SECTOR_SIZE, i);
            j++;
        }
    }
    
    // Write back to disk
    writeSector(&map_fs_buf, FS_MAP_SECTOR_NUMBER);
    writeSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
    writeSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER);
    writeSector(&data_fs_buf, FS_DATA_SECTOR_NUMBER);
    
    *status = FS_W_SUCCESS;
}

// TODO: 6. Implement fsCat function
void fsCat(struct file_metadata* metadata, enum fs_return* status) {
    struct node_fs node_fs_buf;
    struct data_fs data_fs_buf;
    int i, match;
    char sector_buffer[SECTOR_SIZE];
    
    // Read node and data sectors
    readSector(&data_fs_buf, FS_DATA_SECTOR_NUMBER);
    readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
    readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER);
    
    // Find the file
    match = -1;
    for (i = 0; i < FS_MAX_NODE; i++) {
        if (strcmp(node_fs_buf.nodes[i].node_name, metadata->node_name) == 0 && 
            node_fs_buf.nodes[i].parent_index == metadata->parent_index) {
            match = i;
            break;
        }
    }
    
    if (match == -1) {
        *status = FS_R_NODE_NOT_FOUND;
        return;
    }
    
    // Check if it's a file (not directory)
    if (node_fs_buf.nodes[match].data_index == 0xFF) {
        *status = FS_R_TYPE_IS_DIRECTORY;
        return;
    }
    
    // Read and display file content sector by sector
    for (i = 0; i < FS_MAX_SECTOR; i++) {
        if (data_fs_buf.datas[node_fs_buf.nodes[match].data_index].sectors[i] == 0x00) break;
        
        readSector(sector_buffer, data_fs_buf.datas[node_fs_buf.nodes[match].data_index].sectors[i]);
        
        // Print sector content (assuming printString function exists)
        int j;
        for (j = 0; j < SECTOR_SIZE && sector_buffer[j] != '\0'; j++) {
            // Print character by character or use your OS's print function
            // printChar(sector_buffer[j]); // Uncomment if you have printChar function
        }
    }
    
    *status = FS_R_SUCCESS;
}

// TODO: 7. Implement fsMkdir function
void fsMkdir(struct file_metadata* metadata, enum fs_return* status) {
    struct node_fs node_fs_buf;
    int i, match_node;
    
    // Read node sectors
    readSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
    readSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER);
    
    // Check if directory already exists
    for (i = 0; i < FS_MAX_NODE; i++) {
        if (strcmp(node_fs_buf.nodes[i].node_name, metadata->node_name) == 0 && 
            node_fs_buf.nodes[i].parent_index == metadata->parent_index) {
            *status = FS_W_NODE_ALREADY_EXISTS;
            return;
        }
    }
    
    // Find empty node slot
    match_node = -1;
    for (i = 0; i < FS_MAX_NODE; i++) {
        if (node_fs_buf.nodes[i].node_name[0] == '\0') {
            match_node = i;
            break;
        }
    }
    
    if (match_node == -1) {
        *status = FS_W_NO_FREE_NODE;
        return;
    }
    
    // Create directory entry
    strcpy(node_fs_buf.nodes[match_node].node_name, metadata->node_name);
    node_fs_buf.nodes[match_node].parent_index = metadata->parent_index;
    node_fs_buf.nodes[match_node].data_index = 0xFF; // 0xFF indicates directory
    
    // Write back to disk
    writeSector(&(node_fs_buf.nodes[0]), FS_NODE_SECTOR_NUMBER);
    writeSector(&(node_fs_buf.nodes[32]), FS_NODE_SECTOR_NUMBER);
    
    *status = FS_W_SUCCESS;
}
