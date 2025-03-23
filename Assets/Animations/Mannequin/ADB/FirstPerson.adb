<AnimDB FragDef="Animations/Mannequin/ADB/PlayerFragmentIds.xml" TagDef="Animations/Mannequin/ADB/PlayerTags.xml">
 <SubADBs>
  <SubADB Tags="Rotate" File="Animations/Mannequin/ADB/NPCCharacterFat.adb">
   <FragmentID Name="Idle"/>
  </SubADB>
 </SubADBs>
 <FragmentList>
  <Idle>
   <Fragment BlendOutDuration="0.2" Tags="">
    <AnimLayer>
     <Blend ExitTime="0" StartTime="0" Duration="1"/>
     <Animation name="Idle" flags="Loop"/>
    </AnimLayer>
   </Fragment>
   <Fragment BlendOutDuration="0.2" Tags="">
    <AnimLayer>
     <Blend ExitTime="0" StartTime="0" Duration="0.2"/>
     <Animation name="Idle" flags="Loop"/>
    </AnimLayer>
   </Fragment>
  </Idle>
  <Move>
   <Fragment BlendOutDuration="0.2" Tags="">
    <AnimLayer>
     <Blend ExitTime="0.30000001" StartTime="0" Duration="0.2"/>
     <Animation name="locomotion" flags="Loop+FullRootPriority"/>
    </AnimLayer>
   </Fragment>
  </Move>
 </FragmentList>
 <FragmentBlendList>
  <Blend from="Idle" to="Move">
   <Variant from="" to="">
    <Fragment BlendOutDuration="0.2" selectTime="0" enterTime="0" flags="Cyclic">
     <AnimLayer>
      <Blend ExitTime="0" StartTime="0.029999999" Duration="0.050000001" flags="Idle2Move"/>
      <Animation name="Walk"/>
      <Blend ExitTime="-1" StartTime="0" Duration="0.18677664" terminal="1"/>
     </AnimLayer>
    </Fragment>
   </Variant>
  </Blend>
  <Blend from="Move" to="Idle">
   <Variant from="" to="">
    <Fragment BlendOutDuration="0.2" selectTime="0" enterTime="0">
     <AnimLayer>
      <Blend ExitTime="0" StartTime="0" Duration="0.46000004" terminal="1"/>
     </AnimLayer>
    </Fragment>
   </Variant>
  </Blend>
 </FragmentBlendList>
</AnimDB>
