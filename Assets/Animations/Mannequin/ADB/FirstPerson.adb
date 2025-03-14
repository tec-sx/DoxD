<AnimDB FragDef="Animations/Mannequin/ADB/PlayerFragmentIds.xml" TagDef="Animations/Mannequin/ADB/PlayerTags.xml">
 <SubADBs>
  <SubADB Tags="Rotate" File="Animations/Mannequin/ADB/NPCCharacterFat.adb">
   <FragmentID Name="Idle"/>
   <FragmentID Name="Walk"/>
  </SubADB>
 </SubADBs>
 <FragmentList>
  <Idle>
   <Fragment BlendOutDuration="0.2" Tags="">
    <AnimLayer>
     <Blend ExitTime="0" StartTime="0" Duration="0.2"/>
     <Animation name="Idle" flags="Loop"/>
    </AnimLayer>
   </Fragment>
  </Idle>
  <Walk>
   <Fragment BlendOutDuration="0.2" Tags="">
    <AnimLayer>
     <Blend ExitTime="0" StartTime="0" Duration="0.2"/>
     <Animation name="locomotion" flags="Loop"/>
    </AnimLayer>
   </Fragment>
   <Fragment BlendOutDuration="0.2" Tags="">
    <AnimLayer>
     <Blend ExitTime="0" StartTime="0" Duration="0.2"/>
     <Animation name="locomotion" flags="Loop"/>
    </AnimLayer>
   </Fragment>
  </Walk>
 </FragmentList>
</AnimDB>
